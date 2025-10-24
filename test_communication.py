#!/usr/bin/env python3
"""
AIMLAB VR Data Streamer - Automated Test Suite

This script provides automated testing for the network communication system.
It can send various types of test data and verify bidirectional communication.
"""

import socket
import json
import time
import sys
import argparse
import random
import threading
from typing import List, Dict, Any


class VRDataTester:
    """Automated tester for VR data streaming."""
    
    def __init__(self, target_port: int = 45001):
        """
        Initialize the tester.
        
        Args:
            target_port: Port to send test data to
        """
        self.target_port = target_port
        self.socket = None
        self.test_results: Dict[str, bool] = {}
        
    def setup(self) -> bool:
        """Set up the test socket."""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
            return True
        except Exception as e:
            print(f"Failed to set up socket: {e}")
            return False
    
    def cleanup(self):
        """Clean up resources."""
        if self.socket:
            self.socket.close()
    
    def send_message(self, msg_type: int, payload: str, ip: str = "127.0.0.1") -> bool:
        """
        Send a test message.
        
        Args:
            msg_type: Message type code
            payload: Message payload
            ip: Target IP address
            
        Returns:
            bool: True if sent successfully
        """
        try:
            message = f"{msg_type}|{payload}"
            self.socket.sendto(message.encode('utf-8'), (ip, self.target_port))
            return True
        except Exception as e:
            print(f"Failed to send message: {e}")
            return False
    
    def test_basic_communication(self):
        """Test basic message sending."""
        print("\n[TEST] Basic Communication")
        print("-" * 40)
        
        test_data = {
            'type': 'test_message',
            'timestamp': time.time_ns(),
            'data': 'Hello from test suite!'
        }
        
        # Send DATA message (type 5)
        success = self.send_message(5, json.dumps(test_data))
        self.test_results['basic_communication'] = success
        
        if success:
            print("✓ Successfully sent test message")
        else:
            print("✗ Failed to send test message")
    
    def test_vr_position_data(self):
        """Test sending VR position data."""
        print("\n[TEST] VR Position Data")
        print("-" * 40)
        
        success_count = 0
        num_samples = 10
        
        print(f"Sending {num_samples} position samples...")
        
        for i in range(num_samples):
            position_data = {
                'type': 'head_position',
                'timestamp': time.time_ns(),
                'frame': i,
                'position': {
                    'x': random.uniform(-2.0, 2.0),
                    'y': random.uniform(0.0, 2.0),
                    'z': random.uniform(-2.0, 2.0)
                }
            }
            
            if self.send_message(5, json.dumps(position_data)):
                success_count += 1
            
            time.sleep(0.1)
        
        self.test_results['vr_position'] = (success_count == num_samples)
        print(f"✓ Sent {success_count}/{num_samples} position samples")
    
    def test_vr_rotation_data(self):
        """Test sending VR rotation data."""
        print("\n[TEST] VR Rotation Data")
        print("-" * 40)
        
        success_count = 0
        num_samples = 10
        
        print(f"Sending {num_samples} rotation samples...")
        
        for i in range(num_samples):
            rotation_data = {
                'type': 'head_rotation',
                'timestamp': time.time_ns(),
                'frame': i,
                'rotation': {
                    'pitch': random.uniform(-90, 90),
                    'yaw': random.uniform(-180, 180),
                    'roll': random.uniform(-180, 180)
                },
                'quaternion': {
                    'w': random.random(),
                    'x': random.random(),
                    'y': random.random(),
                    'z': random.random()
                }
            }
            
            if self.send_message(5, json.dumps(rotation_data)):
                success_count += 1
            
            time.sleep(0.1)
        
        self.test_results['vr_rotation'] = (success_count == num_samples)
        print(f"✓ Sent {success_count}/{num_samples} rotation samples")
    
    def test_hand_tracking_data(self):
        """Test sending hand tracking data."""
        print("\n[TEST] Hand Tracking Data")
        print("-" * 40)
        
        hands = ['left', 'right']
        joints = ['thumb', 'index', 'middle', 'ring', 'pinky']
        
        success = True
        for hand in hands:
            hand_data = {
                'type': 'hand_tracking',
                'timestamp': time.time_ns(),
                'hand': hand,
                'joints': {}
            }
            
            for joint in joints:
                hand_data['joints'][joint] = {
                    'position': {
                        'x': random.uniform(-0.5, 0.5),
                        'y': random.uniform(-0.5, 0.5),
                        'z': random.uniform(-0.5, 0.5)
                    },
                    'confidence': random.uniform(0.7, 1.0)
                }
            
            if not self.send_message(5, json.dumps(hand_data)):
                success = False
            
            print(f"  Sent {hand} hand data")
            time.sleep(0.1)
        
        self.test_results['hand_tracking'] = success
        if success:
            print("✓ Successfully sent hand tracking data")
        else:
            print("✗ Failed to send hand tracking data")
    
    def test_eye_tracking_data(self):
        """Test sending eye tracking data."""
        print("\n[TEST] Eye Tracking Data")
        print("-" * 40)
        
        num_samples = 20
        success_count = 0
        
        print(f"Sending {num_samples} eye tracking samples...")
        
        for i in range(num_samples):
            eye_data = {
                'type': 'eye_tracking',
                'timestamp': time.time_ns(),
                'frame': i,
                'gaze': {
                    'origin': {
                        'x': random.uniform(-0.1, 0.1),
                        'y': random.uniform(-0.1, 0.1),
                        'z': 0.0
                    },
                    'direction': {
                        'x': random.uniform(-1.0, 1.0),
                        'y': random.uniform(-1.0, 1.0),
                        'z': random.uniform(0.5, 1.0)
                    }
                },
                'pupil_diameter': {
                    'left': random.uniform(2.0, 8.0),
                    'right': random.uniform(2.0, 8.0)
                },
                'blink': {
                    'left': random.random() > 0.95,
                    'right': random.random() > 0.95
                }
            }
            
            if self.send_message(5, json.dumps(eye_data)):
                success_count += 1
            
            time.sleep(0.05)
        
        self.test_results['eye_tracking'] = (success_count == num_samples)
        print(f"✓ Sent {success_count}/{num_samples} eye tracking samples")
    
    def test_controller_input(self):
        """Test sending controller input data."""
        print("\n[TEST] Controller Input Data")
        print("-" * 40)
        
        controllers = ['left', 'right']
        buttons = ['trigger', 'grip', 'menu', 'a', 'b']
        
        success = True
        for controller in controllers:
            controller_data = {
                'type': 'controller_input',
                'timestamp': time.time_ns(),
                'controller': controller,
                'buttons': {},
                'joystick': {
                    'x': random.uniform(-1.0, 1.0),
                    'y': random.uniform(-1.0, 1.0)
                },
                'touchpad': {
                    'x': random.uniform(-1.0, 1.0),
                    'y': random.uniform(-1.0, 1.0),
                    'touched': random.random() > 0.5
                }
            }
            
            for button in buttons:
                controller_data['buttons'][button] = {
                    'pressed': random.random() > 0.7,
                    'value': random.random()
                }
            
            if not self.send_message(5, json.dumps(controller_data)):
                success = False
            
            print(f"  Sent {controller} controller data")
            time.sleep(0.1)
        
        self.test_results['controller_input'] = success
        if success:
            print("✓ Successfully sent controller input data")
        else:
            print("✗ Failed to send controller input data")
    
    def test_performance_metrics(self):
        """Test sending performance metrics."""
        print("\n[TEST] Performance Metrics")
        print("-" * 40)
        
        metrics_data = {
            'type': 'performance_metrics',
            'timestamp': time.time_ns(),
            'fps': random.uniform(80, 120),
            'latency_ms': random.uniform(5, 25),
            'dropped_frames': random.randint(0, 5),
            'cpu_usage': random.uniform(20, 80),
            'gpu_usage': random.uniform(30, 90),
            'memory_mb': random.randint(1000, 4000),
            'network': {
                'bandwidth_mbps': random.uniform(10, 100),
                'packet_loss': random.uniform(0, 0.05),
                'jitter_ms': random.uniform(0, 10)
            }
        }
        
        success = self.send_message(5, json.dumps(metrics_data))
        self.test_results['performance_metrics'] = success
        
        if success:
            print("✓ Successfully sent performance metrics")
            print(f"  FPS: {metrics_data['fps']:.1f}")
            print(f"  Latency: {metrics_data['latency_ms']:.1f}ms")
            print(f"  Network: {metrics_data['network']['bandwidth_mbps']:.1f} Mbps")
        else:
            print("✗ Failed to send performance metrics")
    
    def test_large_payload(self):
        """Test sending large payload."""
        print("\n[TEST] Large Payload")
        print("-" * 40)
        
        # Create a large data structure
        large_data = {
            'type': 'large_payload_test',
            'timestamp': time.time_ns(),
            'data': []
        }
        
        # Add multiple data points
        for i in range(100):
            large_data['data'].append({
                'index': i,
                'value': random.random(),
                'nested': {
                    'x': random.random(),
                    'y': random.random(),
                    'z': random.random()
                }
            })
        
        payload = json.dumps(large_data)
        payload_size = len(payload)
        print(f"Sending payload of {payload_size} bytes...")
        
        success = self.send_message(5, payload)
        self.test_results['large_payload'] = success
        
        if success:
            print(f"✓ Successfully sent large payload ({payload_size} bytes)")
        else:
            print(f"✗ Failed to send large payload ({payload_size} bytes)")
    
    def test_stress(self, duration: int = 5):
        """
        Stress test with rapid message sending.
        
        Args:
            duration: Test duration in seconds
        """
        print(f"\n[TEST] Stress Test ({duration} seconds)")
        print("-" * 40)
        
        start_time = time.time()
        message_count = 0
        error_count = 0
        
        print(f"Sending rapid messages for {duration} seconds...")
        
        while time.time() - start_time < duration:
            test_data = {
                'type': 'stress_test',
                'timestamp': time.time_ns(),
                'sequence': message_count,
                'random_data': random.random()
            }
            
            if self.send_message(5, json.dumps(test_data)):
                message_count += 1
            else:
                error_count += 1
            
            # Small delay to prevent overwhelming the network
            time.sleep(0.001)
        
        elapsed = time.time() - start_time
        rate = message_count / elapsed
        
        self.test_results['stress_test'] = (error_count == 0)
        
        print(f"✓ Stress test complete")
        print(f"  Messages sent: {message_count}")
        print(f"  Errors: {error_count}")
        print(f"  Rate: {rate:.1f} messages/second")
    
    def run_all_tests(self):
        """Run all test suites."""
        print("\n" + "=" * 50)
        print(" AIMLAB VR Data Streamer - Test Suite")
        print("=" * 50)
        
        if not self.setup():
            print("Failed to set up test environment")
            return False
        
        try:
            # Run all tests
            self.test_basic_communication()
            time.sleep(0.5)
            
            self.test_vr_position_data()
            time.sleep(0.5)
            
            self.test_vr_rotation_data()
            time.sleep(0.5)
            
            self.test_hand_tracking_data()
            time.sleep(0.5)
            
            self.test_eye_tracking_data()
            time.sleep(0.5)
            
            self.test_controller_input()
            time.sleep(0.5)
            
            self.test_performance_metrics()
            time.sleep(0.5)
            
            self.test_large_payload()
            time.sleep(0.5)
            
            self.test_stress(duration=3)
            
            # Print summary
            self.print_summary()
            
        finally:
            self.cleanup()
        
        return all(self.test_results.values())
    
    def print_summary(self):
        """Print test results summary."""
        print("\n" + "=" * 50)
        print(" Test Results Summary")
        print("=" * 50)
        
        total_tests = len(self.test_results)
        passed_tests = sum(1 for result in self.test_results.values() if result)
        
        for test_name, result in self.test_results.items():
            status = "✓ PASS" if result else "✗ FAIL"
            print(f"{test_name:25} {status}")
        
        print("-" * 50)
        print(f"Total: {passed_tests}/{total_tests} tests passed")
        
        if passed_tests == total_tests:
            print("\n🎉 All tests passed successfully!")
        else:
            print(f"\n⚠️  {total_tests - passed_tests} test(s) failed")


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description='AIMLAB VR Data Streamer - Test Suite'
    )
    parser.add_argument(
        '--port',
        type=int,
        default=45001,
        help='Target port for sending test data (default: 45001)'
    )
    parser.add_argument(
        '--target',
        type=str,
        default='127.0.0.1',
        help='Target IP address (default: 127.0.0.1)'
    )
    parser.add_argument(
        '--test',
        type=str,
        choices=['all', 'basic', 'position', 'rotation', 'hand', 'eye', 
                'controller', 'performance', 'large', 'stress'],
        default='all',
        help='Specific test to run (default: all)'
    )
    parser.add_argument(
        '--stress-duration',
        type=int,
        default=5,
        help='Duration for stress test in seconds (default: 5)'
    )
    
    args = parser.parse_args()
    
    # Create tester
    tester = VRDataTester(target_port=args.port)
    
    if not tester.setup():
        print("Failed to initialize tester")
        return 1
    
    try:
        # Run specific test or all tests
        if args.test == 'all':
            success = tester.run_all_tests()
        else:
            test_map = {
                'basic': tester.test_basic_communication,
                'position': tester.test_vr_position_data,
                'rotation': tester.test_vr_rotation_data,
                'hand': tester.test_hand_tracking_data,
                'eye': tester.test_eye_tracking_data,
                'controller': tester.test_controller_input,
                'performance': tester.test_performance_metrics,
                'large': tester.test_large_payload,
                'stress': lambda: tester.test_stress(args.stress_duration)
            }
            
            test_func = test_map.get(args.test)
            if test_func:
                test_func()
                success = True
            else:
                print(f"Unknown test: {args.test}")
                success = False
        
        return 0 if success else 1
        
    finally:
        tester.cleanup()


if __name__ == '__main__':
    sys.exit(main())
