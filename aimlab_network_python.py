#!/usr/bin/env python3
"""
AIMLAB VR Data Streamer - Python Network Module

This module provides a robust UDP-based network communication system with:
- Automatic peer discovery on local network
- Reliable handshake protocol  
- Bidirectional communication
- Heartbeat mechanism for connection monitoring
- Thread-safe message queuing
- Asyncio-based event-driven architecture

Author: AIMLAB Development Team
Version: 1.0.0

Requirements:
    - Python 3.7+
    - No external dependencies (uses standard library only)

Usage:
    python aimlab_network_python.py [--no-discovery]
"""

import socket
import struct
import json
import threading
import queue
import time
import sys
import os
import logging
import argparse
import asyncio
import ipaddress
from dataclasses import dataclass, field
from enum import IntEnum
from typing import Dict, List, Optional, Callable, Tuple, Any
from datetime import datetime, timedelta
import traceback


# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger('AIMLAB_VR')


class NetworkConfig:
    """Configuration parameters for the network module."""
    
    DISCOVERY_PORT: int = 45000
    DEFAULT_DATA_PORT: int = 45001
    BUFFER_SIZE: int = 4096
    DISCOVERY_INTERVAL_MS: int = 1000
    HEARTBEAT_INTERVAL_MS: int = 5000
    CONNECTION_TIMEOUT_MS: int = 15000
    PROTOCOL_VERSION: str = "1.0"
    APP_IDENTIFIER: str = "AIMLAB_VR"
    
    # Network settings
    BROADCAST_ADDRESS: str = "255.255.255.255"
    MAX_PEERS: int = 10
    RECV_TIMEOUT: float = 0.1
    
    @classmethod
    def to_dict(cls) -> dict:
        """Convert configuration to dictionary."""
        return {
            key: getattr(cls, key) 
            for key in dir(cls) 
            if not key.startswith('_') and not callable(getattr(cls, key))
        }


class MessageType(IntEnum):
    """Types of messages in the protocol."""
    
    DISCOVER = 0
    ACKNOWLEDGE = 1
    HANDSHAKE_START = 2
    HANDSHAKE_ACK = 3
    HANDSHAKE_COMPLETE = 4
    DATA = 5
    HEARTBEAT = 6
    DISCONNECT = 7
    ERROR = 8


@dataclass
class Message:
    """Represents a network message with type and payload."""
    
    type: MessageType
    payload: str = ""
    sender_ip: str = ""
    sender_port: int = 0
    timestamp: float = field(default_factory=time.time)
    
    def serialize(self) -> bytes:
        """
        Serialize message to bytes for transmission.
        
        Returns:
            bytes: Serialized message
        """
        message_str = f"{int(self.type)}|{self.payload}"
        return message_str.encode('utf-8')
    
    @classmethod
    def deserialize(cls, data: bytes) -> 'Message':
        """
        Deserialize message from bytes.
        
        Args:
            data: Raw message bytes
            
        Returns:
            Message: Deserialized message object
        """
        try:
            message_str = data.decode('utf-8')
            parts = message_str.split('|', 1)
            if len(parts) >= 2:
                msg_type = MessageType(int(parts[0]))
                payload = parts[1]
            else:
                msg_type = MessageType.ERROR
                payload = ""
            return cls(type=msg_type, payload=payload)
        except Exception as e:
            logger.error(f"Failed to deserialize message: {e}")
            return cls(type=MessageType.ERROR, payload=str(e))


@dataclass
class PeerInfo:
    """Information about a connected peer."""
    
    ip_address: str
    port: int
    last_heartbeat: float = field(default_factory=time.time)
    is_connected: bool = False
    protocol_version: str = ""
    
    def is_timeout(self) -> bool:
        """
        Check if peer connection has timed out.
        
        Returns:
            bool: True if timed out, False otherwise
        """
        elapsed = (time.time() - self.last_heartbeat) * 1000
        return elapsed > NetworkConfig.CONNECTION_TIMEOUT_MS
    
    def update_heartbeat(self):
        """Update the last heartbeat timestamp."""
        self.last_heartbeat = time.time()
    
    @property
    def peer_id(self) -> str:
        """Get unique identifier for this peer."""
        return f"{self.ip_address}:{self.port}"


class UDPSocket:
    """Wrapper class for UDP socket operations."""
    
    def __init__(self):
        """Initialize UDP socket wrapper."""
        self.socket: Optional[socket.socket] = None
        self.is_valid: bool = False
        self.local_port: int = 0
        
    def __enter__(self):
        """Context manager entry."""
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.close()
    
    def initialize(self, port: int = 0) -> bool:
        """
        Initialize socket and bind to specified port.
        
        Args:
            port: Port number to bind (0 for auto-assignment)
            
        Returns:
            bool: True if successful, False otherwise
        """
        try:
            # Create UDP socket
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
            
            # Enable broadcast
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
            
            # Enable address reuse
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            
            # Set non-blocking mode
            self.socket.setblocking(False)
            
            # Bind to port
            self.socket.bind(('', port))
            
            # Get actual port if auto-assigned
            self.local_port = self.socket.getsockname()[1]
            
            self.is_valid = True
            logger.info(f"Socket initialized on port {self.local_port}")
            return True
            
        except Exception as e:
            logger.error(f"Failed to initialize socket: {e}")
            if self.socket:
                self.socket.close()
            return False
    
    def send_to(self, data: bytes, ip: str, port: int) -> int:
        """
        Send data to specified address.
        
        Args:
            data: Data to send
            ip: Target IP address
            port: Target port
            
        Returns:
            int: Number of bytes sent, -1 on error
        """
        if not self.is_valid:
            return -1
            
        try:
            return self.socket.sendto(data, (ip, port))
        except Exception as e:
            logger.error(f"Failed to send data to {ip}:{port}: {e}")
            return -1
    
    def broadcast(self, data: bytes, port: int) -> int:
        """
        Broadcast data to all hosts on local network.
        
        Args:
            data: Data to broadcast
            port: Target port
            
        Returns:
            int: Number of bytes sent, -1 on error
        """
        return self.send_to(data, NetworkConfig.BROADCAST_ADDRESS, port)
    
    def receive_from(self, buffer_size: int = NetworkConfig.BUFFER_SIZE) -> Optional[Tuple[bytes, str, int]]:
        """
        Receive data from socket (non-blocking).
        
        Args:
            buffer_size: Size of receive buffer
            
        Returns:
            Tuple of (data, sender_ip, sender_port) or None if no data
        """
        if not self.is_valid:
            return None
            
        try:
            data, (ip, port) = self.socket.recvfrom(buffer_size)
            return data, ip, port
        except socket.error:
            # No data available (non-blocking)
            return None
        except Exception as e:
            logger.error(f"Failed to receive data: {e}")
            return None
    
    def close(self):
        """Close the socket."""
        if self.socket:
            try:
                self.socket.close()
            except:
                pass
            self.socket = None
            self.is_valid = False


class NetworkManager:
    """Main class managing network operations and peer connections."""
    
    def __init__(self):
        """Initialize the network manager."""
        self.discovery_socket = UDPSocket()
        self.data_socket = UDPSocket()
        self.peers: Dict[str, PeerInfo] = {}
        self.incoming_messages = queue.Queue()
        self.outgoing_messages = queue.Queue()
        
        self._peers_lock = threading.RLock()
        self._running = False
        self._discovering = False
        
        self._threads: List[threading.Thread] = []
        self.message_handler: Optional[Callable[[Message], None]] = None
        
    def start(self, enable_discovery: bool = True) -> bool:
        """
        Start the network manager.
        
        Args:
            enable_discovery: Enable automatic peer discovery
            
        Returns:
            bool: True if successful, False otherwise
        """
        if self._running:
            logger.warning("Network manager already running")
            return False
        
        # Initialize sockets
        if not self.discovery_socket.initialize(NetworkConfig.DISCOVERY_PORT):
            logger.error("Failed to initialize discovery socket")
            return False
        
        if not self.data_socket.initialize(NetworkConfig.DEFAULT_DATA_PORT):
            logger.error("Failed to initialize data socket")
            self.discovery_socket.close()
            return False
        
        self._running = True
        self._discovering = enable_discovery
        
        # Start worker threads
        if enable_discovery:
            self._start_thread(self._discovery_worker, "Discovery")
        
        self._start_thread(self._receive_worker, "Receive")
        self._start_thread(self._send_worker, "Send")
        self._start_thread(self._heartbeat_worker, "Heartbeat")
        
        logger.info(f"Network manager started on port {self.data_socket.local_port}")
        return True
    
    def stop(self):
        """Stop the network manager."""
        if not self._running:
            return
        
        logger.info("Stopping network manager...")
        
        # Send disconnect messages
        self._broadcast_disconnect()
        
        # Stop all threads
        self._running = False
        self._discovering = False
        
        # Wait for threads to finish
        for thread in self._threads:
            thread.join(timeout=2.0)
        
        # Close sockets
        self.discovery_socket.close()
        self.data_socket.close()
        
        logger.info("Network manager stopped")
    
    def send_to_peer(self, ip_address: str, port: int, data: str):
        """
        Send a message to a specific peer.
        
        Args:
            ip_address: Peer's IP address
            port: Peer's port
            data: Data to send
        """
        msg = Message(MessageType.DATA, data, ip_address, port)
        self.outgoing_messages.put(msg)
    
    def broadcast_to_peers(self, data: str):
        """
        Broadcast a message to all connected peers.
        
        Args:
            data: Data to broadcast
        """
        with self._peers_lock:
            for peer_info in self.peers.values():
                if peer_info.is_connected:
                    self.send_to_peer(peer_info.ip_address, peer_info.port, data)
    
    def get_message(self) -> Optional[Message]:
        """
        Get next message from incoming queue.
        
        Returns:
            Message object or None if queue is empty
        """
        try:
            return self.incoming_messages.get_nowait()
        except queue.Empty:
            return None
    
    def set_message_handler(self, handler: Callable[[Message], None]):
        """
        Set custom message handler callback.
        
        Args:
            handler: Function to handle incoming messages
        """
        self.message_handler = handler
    
    def get_connected_peers(self) -> List[PeerInfo]:
        """
        Get list of connected peers.
        
        Returns:
            List of peer information objects
        """
        with self._peers_lock:
            return [
                peer for peer in self.peers.values()
                if peer.is_connected and not peer.is_timeout()
            ]
    
    def _start_thread(self, target: Callable, name: str):
        """Start a worker thread."""
        thread = threading.Thread(target=target, name=name, daemon=True)
        thread.start()
        self._threads.append(thread)
    
    def _discovery_worker(self):
        """Worker thread for peer discovery."""
        logger.info("Discovery worker started")
        
        while self._discovering:
            try:
                # Send discovery broadcast
                discovery_msg = f"{NetworkConfig.APP_IDENTIFIER}:{self.data_socket.local_port}"
                msg = Message(MessageType.DISCOVER, discovery_msg)
                self.discovery_socket.broadcast(msg.serialize(), NetworkConfig.DISCOVERY_PORT)
                
                # Check for discovery responses
                result = self.discovery_socket.receive_from()
                while result:
                    data, sender_ip, sender_port = result
                    self._handle_discovery_message(data, sender_ip)
                    result = self.discovery_socket.receive_from()
                
            except Exception as e:
                logger.error(f"Discovery worker error: {e}")
            
            time.sleep(NetworkConfig.DISCOVERY_INTERVAL_MS / 1000.0)
        
        logger.info("Discovery worker stopped")
    
    def _receive_worker(self):
        """Worker thread for receiving messages."""
        logger.info("Receive worker started")
        
        while self._running:
            try:
                result = self.data_socket.receive_from()
                if result:
                    data, sender_ip, sender_port = result
                    msg = Message.deserialize(data)
                    msg.sender_ip = sender_ip
                    msg.sender_port = sender_port
                    self._handle_incoming_message(msg)
                
            except Exception as e:
                logger.error(f"Receive worker error: {e}")
            
            time.sleep(0.01)
        
        logger.info("Receive worker stopped")
    
    def _send_worker(self):
        """Worker thread for sending messages."""
        logger.info("Send worker started")
        
        while self._running:
            try:
                msg = self.outgoing_messages.get(timeout=0.1)
                serialized = msg.serialize()
                self.data_socket.send_to(serialized, msg.sender_ip, msg.sender_port)
                
            except queue.Empty:
                continue
            except Exception as e:
                logger.error(f"Send worker error: {e}")
        
        logger.info("Send worker stopped")
    
    def _heartbeat_worker(self):
        """Worker thread for sending heartbeats."""
        logger.info("Heartbeat worker started")
        
        while self._running:
            try:
                with self._peers_lock:
                    # Check for timeouts and send heartbeats
                    for peer_id, peer_info in list(self.peers.items()):
                        if peer_info.is_connected:
                            if peer_info.is_timeout():
                                logger.warning(f"Peer timeout: {peer_id}")
                                peer_info.is_connected = False
                            else:
                                # Send heartbeat
                                heartbeat = Message(MessageType.HEARTBEAT, "")
                                heartbeat.sender_ip = peer_info.ip_address
                                heartbeat.sender_port = peer_info.port
                                self.outgoing_messages.put(heartbeat)
                
            except Exception as e:
                logger.error(f"Heartbeat worker error: {e}")
            
            time.sleep(NetworkConfig.HEARTBEAT_INTERVAL_MS / 1000.0)
        
        logger.info("Heartbeat worker stopped")
    
    def _handle_discovery_message(self, data: bytes, sender_ip: str):
        """Handle discovery messages."""
        try:
            msg = Message.deserialize(data)
            
            # Parse discovery message
            if NetworkConfig.APP_IDENTIFIER in msg.payload:
                parts = msg.payload.split(':')
                if len(parts) >= 2:
                    peer_port = int(parts[-1])
                    peer_id = f"{sender_ip}:{peer_port}"
                    
                    with self._peers_lock:
                        # Check if peer is new
                        if peer_id not in self.peers:
                            logger.info(f"Discovered new peer: {peer_id}")
                            
                            # Add peer
                            self.peers[peer_id] = PeerInfo(sender_ip, peer_port)
                            
                            # Send acknowledge if this was a discovery message
                            if msg.type == MessageType.DISCOVER:
                                ack_msg = f"{NetworkConfig.APP_IDENTIFIER}:{self.data_socket.local_port}"
                                ack = Message(MessageType.ACKNOWLEDGE, ack_msg)
                                self.discovery_socket.send_to(
                                    ack.serialize(), sender_ip, NetworkConfig.DISCOVERY_PORT
                                )
                                
                                # Initiate handshake
                                self._initiate_handshake(sender_ip, peer_port)
                    
        except Exception as e:
            logger.error(f"Error handling discovery message: {e}")
    
    def _handle_incoming_message(self, msg: Message):
        """Handle incoming data messages."""
        peer_id = f"{msg.sender_ip}:{msg.sender_port}"
        
        # Update peer heartbeat
        with self._peers_lock:
            if peer_id in self.peers:
                self.peers[peer_id].update_heartbeat()
        
        # Process message based on type
        if msg.type == MessageType.HANDSHAKE_START:
            self._handle_handshake_start(msg)
        elif msg.type == MessageType.HANDSHAKE_ACK:
            self._handle_handshake_ack(msg)
        elif msg.type == MessageType.HANDSHAKE_COMPLETE:
            self._handle_handshake_complete(msg)
        elif msg.type == MessageType.DATA:
            # Add to incoming queue
            self.incoming_messages.put(msg)
            # Call custom handler if set
            if self.message_handler:
                try:
                    self.message_handler(msg)
                except Exception as e:
                    logger.error(f"Message handler error: {e}")
        elif msg.type == MessageType.HEARTBEAT:
            # Heartbeat already handled above
            pass
        elif msg.type == MessageType.DISCONNECT:
            self._handle_disconnect(msg)
    
    def _initiate_handshake(self, ip: str, port: int):
        """Initiate handshake with peer."""
        logger.info(f"Initiating handshake with {ip}:{port}")
        
        handshake = Message(MessageType.HANDSHAKE_START, NetworkConfig.PROTOCOL_VERSION)
        handshake.sender_ip = ip
        handshake.sender_port = port
        self.outgoing_messages.put(handshake)
    
    def _handle_handshake_start(self, msg: Message):
        """Handle handshake start message."""
        logger.info(f"Received handshake start from {msg.sender_ip}:{msg.sender_port}")
        
        # Send handshake acknowledgment
        ack = Message(MessageType.HANDSHAKE_ACK, NetworkConfig.PROTOCOL_VERSION)
        ack.sender_ip = msg.sender_ip
        ack.sender_port = msg.sender_port
        self.outgoing_messages.put(ack)
    
    def _handle_handshake_ack(self, msg: Message):
        """Handle handshake acknowledgment."""
        logger.info(f"Received handshake ack from {msg.sender_ip}:{msg.sender_port}")
        
        # Send handshake complete
        complete = Message(MessageType.HANDSHAKE_COMPLETE, "")
        complete.sender_ip = msg.sender_ip
        complete.sender_port = msg.sender_port
        self.outgoing_messages.put(complete)
        
        # Mark peer as connected
        self._mark_peer_connected(msg.sender_ip, msg.sender_port)
    
    def _handle_handshake_complete(self, msg: Message):
        """Handle handshake complete message."""
        logger.info(f"Handshake complete with {msg.sender_ip}:{msg.sender_port}")
        
        # Mark peer as connected
        self._mark_peer_connected(msg.sender_ip, msg.sender_port)
    
    def _mark_peer_connected(self, ip: str, port: int):
        """Mark peer as connected."""
        peer_id = f"{ip}:{port}"
        
        with self._peers_lock:
            if peer_id in self.peers:
                self.peers[peer_id].is_connected = True
                self.peers[peer_id].update_heartbeat()
                logger.info(f"Peer connected: {peer_id}")
    
    def _handle_disconnect(self, msg: Message):
        """Handle disconnect message."""
        peer_id = f"{msg.sender_ip}:{msg.sender_port}"
        
        with self._peers_lock:
            if peer_id in self.peers:
                del self.peers[peer_id]
        
        logger.info(f"Peer disconnected: {peer_id}")
    
    def _broadcast_disconnect(self):
        """Broadcast disconnect message to all peers."""
        disconnect = Message(MessageType.DISCONNECT, "")
        
        with self._peers_lock:
            for peer_info in self.peers.values():
                if peer_info.is_connected:
                    disconnect.sender_ip = peer_info.ip_address
                    disconnect.sender_port = peer_info.port
                    serialized = disconnect.serialize()
                    self.data_socket.send_to(serialized, peer_info.ip_address, peer_info.port)


class VRDataStreamer:
    """High-level interface for VR data streaming."""
    
    def __init__(self):
        """Initialize the VR data streamer."""
        self.network = NetworkManager()
        self._running = False
        self._input_thread: Optional[threading.Thread] = None
        
        # VR Data simulation parameters
        self.vr_data_types = ['position', 'rotation', 'hand_tracking', 'eye_tracking']
        self.simulation_enabled = False
        
    def start(self, enable_discovery: bool = True) -> bool:
        """
        Start the VR data streamer.
        
        Args:
            enable_discovery: Enable automatic peer discovery
            
        Returns:
            bool: True if successful, False otherwise
        """
        if self._running:
            return False
        
        # Set up message handler
        self.network.set_message_handler(self._handle_vr_data)
        
        # Start network manager
        if not self.network.start(enable_discovery):
            return False
        
        self._running = True
        self._input_thread = threading.Thread(target=self._input_worker, daemon=True)
        self._input_thread.start()
        
        self._print_banner()
        
        return True
    
    def stop(self):
        """Stop the VR data streamer."""
        if not self._running:
            return
        
        self._running = False
        if self._input_thread:
            self._input_thread.join(timeout=2.0)
        
        self.network.stop()
        logger.info("VR Data Streamer stopped")
    
    def run(self):
        """Run the main event loop."""
        try:
            while self._running:
                # Process incoming messages
                msg = self.network.get_message()
                while msg:
                    # Messages are already handled by the callback
                    msg = self.network.get_message()
                
                # Simulate VR data if enabled
                if self.simulation_enabled:
                    self._send_simulated_vr_data()
                
                time.sleep(0.1)
                
        except KeyboardInterrupt:
            logger.info("Received interrupt signal")
        except Exception as e:
            logger.error(f"Main loop error: {e}")
            traceback.print_exc()
    
    def send_vr_data(self, data: dict):
        """
        Send VR data to all connected peers.
        
        Args:
            data: VR data dictionary
        """
        try:
            json_data = json.dumps(data)
            self.network.broadcast_to_peers(json_data)
        except Exception as e:
            logger.error(f"Failed to send VR data: {e}")
    
    def _handle_vr_data(self, msg: Message):
        """Handle incoming VR data."""
        if msg.type == MessageType.DATA:
            try:
                # Parse VR data (expecting JSON)
                vr_data = json.loads(msg.payload)
                
                # Log received data
                data_type = vr_data.get('type', 'unknown')
                timestamp = vr_data.get('timestamp', 0)
                
                print(f"\n[VR Data from {msg.sender_ip}:{msg.sender_port}]")
                print(f"  Type: {data_type}")
                print(f"  Timestamp: {datetime.fromtimestamp(timestamp / 1e9) if timestamp else 'N/A'}")
                print(f"  Data: {vr_data.get('data', 'N/A')}")
                
                # Here you would process VR-specific data
                # For example: update visualization, apply transformations, etc.
                
            except json.JSONDecodeError:
                # Handle non-JSON messages
                print(f"\n[Message from {msg.sender_ip}:{msg.sender_port}] {msg.payload}")
            except Exception as e:
                logger.error(f"Error handling VR data: {e}")
    
    def _send_simulated_vr_data(self):
        """Send simulated VR data for testing."""
        import random
        
        vr_data = {
            'type': random.choice(self.vr_data_types),
            'timestamp': time.time_ns(),
            'data': {
                'x': random.uniform(-10, 10),
                'y': random.uniform(-10, 10),
                'z': random.uniform(-10, 10),
                'confidence': random.uniform(0.8, 1.0)
            }
        }
        
        self.send_vr_data(vr_data)
    
    def _input_worker(self):
        """Worker thread for handling user input."""
        while self._running:
            try:
                user_input = input().strip()
                
                if user_input == 'quit' or user_input == 'exit':
                    self._running = False
                    
                elif user_input == 'peers':
                    self._show_peers()
                    
                elif user_input.startswith('send '):
                    message = user_input[5:]
                    if message:
                        self._send_custom_message(message)
                        
                elif user_input == 'simulate':
                    self.simulation_enabled = not self.simulation_enabled
                    status = "enabled" if self.simulation_enabled else "disabled"
                    print(f"VR data simulation {status}")
                    
                elif user_input == 'status':
                    self._show_status()
                    
                elif user_input == 'help':
                    self._print_help()
                    
                elif user_input:
                    print(f"Unknown command: {user_input}. Type 'help' for available commands.")
                    
            except EOFError:
                # Handle Ctrl+D
                self._running = False
            except Exception as e:
                logger.error(f"Input worker error: {e}")
    
    def _send_custom_message(self, message: str):
        """Send a custom message to all peers."""
        vr_data = {
            'type': 'custom_message',
            'timestamp': time.time_ns(),
            'data': message
        }
        
        self.send_vr_data(vr_data)
        print(f"Sent: {json.dumps(vr_data)}")
    
    def _show_peers(self):
        """Display connected peers."""
        peers = self.network.get_connected_peers()
        print(f"\nConnected peers ({len(peers)}):")
        if peers:
            for peer in peers:
                print(f"  - {peer.ip_address}:{peer.port}")
                print(f"    Last heartbeat: {datetime.fromtimestamp(peer.last_heartbeat)}")
        else:
            print("  No connected peers")
    
    def _show_status(self):
        """Display system status."""
        peers = self.network.get_connected_peers()
        print("\n=== System Status ===")
        print(f"Network: {'Running' if self._running else 'Stopped'}")
        print(f"Data port: {self.network.data_socket.local_port}")
        print(f"Connected peers: {len(peers)}")
        print(f"Simulation: {'Enabled' if self.simulation_enabled else 'Disabled'}")
        print(f"Protocol version: {NetworkConfig.PROTOCOL_VERSION}")
        print("====================")
    
    def _print_banner(self):
        """Print application banner."""
        print("\n" + "=" * 50)
        print(" " * 10 + "AIMLAB VR Data Streamer")
        print(" " * 15 + "Python Edition")
        print("=" * 50)
        print(f"Port: {self.network.data_socket.local_port}")
        print(f"Protocol: v{NetworkConfig.PROTOCOL_VERSION}")
        print("=" * 50)
        self._print_help()
    
    def _print_help(self):
        """Print help information."""
        print("\nCommands:")
        print("  'send <message>' - Send message to all peers")
        print("  'peers'         - List connected peers")
        print("  'simulate'      - Toggle VR data simulation")
        print("  'status'        - Show system status")
        print("  'help'          - Show this help message")
        print("  'quit'          - Exit application")
        print("")


def main():
    """Main entry point."""
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description='AIMLAB VR Data Streamer - Python Network Module'
    )
    parser.add_argument(
        '--no-discovery',
        action='store_true',
        help='Disable automatic peer discovery'
    )
    parser.add_argument(
        '--debug',
        action='store_true',
        help='Enable debug logging'
    )
    parser.add_argument(
        '--port',
        type=int,
        default=NetworkConfig.DEFAULT_DATA_PORT,
        help=f'Data port (default: {NetworkConfig.DEFAULT_DATA_PORT})'
    )
    
    args = parser.parse_args()
    
    # Configure logging level
    if args.debug:
        logging.getLogger().setLevel(logging.DEBUG)
    
    # Update port if specified
    if args.port:
        NetworkConfig.DEFAULT_DATA_PORT = args.port
    
    # Create and start the streamer
    streamer = VRDataStreamer()
    
    if not streamer.start(enable_discovery=not args.no_discovery):
        logger.error("Failed to start VR Data Streamer")
        return 1
    
    try:
        # Run main event loop
        streamer.run()
    finally:
        # Cleanup
        streamer.stop()
    
    return 0


if __name__ == '__main__':
    sys.exit(main())
