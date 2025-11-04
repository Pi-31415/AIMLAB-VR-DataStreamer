"""
@file analyze_vr_data.py
@brief VR Data Analysis Script for AIMLAB experiments
@author Pi Ko (pi.ko@nyu.edu)
@date 04 November 2025
@version v1.0

Python script for analyzing VR tracking data recorded by AIMLAB console application.
Generates visualizations and statistics from CSV files.

Requirements:
    pip install pandas matplotlib numpy

Usage:
    python analyze_vr_data.py <filename.csv>

Example:
    python analyze_vr_data.py session_01.csv

Changelog:
v1.0 - 04 November 2025 - Initial implementation
  - CSV data loading and validation
  - Position and rotation trajectory plotting
  - Movement statistics calculation
  - Distance and velocity analysis
"""

import sys
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

def load_vr_data(filename):
    """
    Load VR tracking data from CSV file
    
    Args:
        filename (str): Path to CSV file
        
    Returns:
        pandas.DataFrame: Loaded VR data
    """
    try:
        df = pd.read_csv(filename)
        print(f"✓ Loaded {filename}")
        print(f"  - Records: {len(df)}")
        print(f"  - Duration: {df['Timestamp'].max() / 1000:.2f} seconds")
        print(f"  - Sampling rate: {len(df) / (df['Timestamp'].max() / 1000):.1f} Hz")
        return df
    except Exception as e:
        print(f"✗ Error loading file: {e}")
        sys.exit(1)

def calculate_distance(df, prefix):
    """
    Calculate total distance traveled for a tracked object
    
    Args:
        df (pandas.DataFrame): VR data
        prefix (str): Column prefix (Head, LeftHand, RightHand)
        
    Returns:
        float: Total distance in meters
    """
    x = df[f'{prefix}PosX'].values
    y = df[f'{prefix}PosY'].values
    z = df[f'{prefix}PosZ'].values
    
    # Calculate distance between consecutive points
    dx = np.diff(x)
    dy = np.diff(y)
    dz = np.diff(z)
    
    distances = np.sqrt(dx**2 + dy**2 + dz**2)
    return np.sum(distances)

def calculate_velocity(df, prefix):
    """
    Calculate average velocity for a tracked object
    
    Args:
        df (pandas.DataFrame): VR data
        prefix (str): Column prefix (Head, LeftHand, RightHand)
        
    Returns:
        tuple: (mean_velocity, max_velocity) in m/s
    """
    x = df[f'{prefix}PosX'].values
    y = df[f'{prefix}PosY'].values
    z = df[f'{prefix}PosZ'].values
    timestamps = df['Timestamp'].values / 1000.0  # Convert to seconds
    
    # Calculate distance between consecutive points
    dx = np.diff(x)
    dy = np.diff(y)
    dz = np.diff(z)
    dt = np.diff(timestamps)
    
    distances = np.sqrt(dx**2 + dy**2 + dz**2)
    velocities = distances / dt
    
    return np.mean(velocities), np.max(velocities)

def plot_trajectories(df, output_dir):
    """
    Plot 3D trajectories of head and controllers
    
    Args:
        df (pandas.DataFrame): VR data
        output_dir (Path): Directory to save plots
    """
    fig = plt.figure(figsize=(15, 5))
    
    # Head trajectory
    ax1 = fig.add_subplot(131, projection='3d')
    ax1.plot(df['HeadPosX'], df['HeadPosY'], df['HeadPosZ'], 
             label='Head', color='blue', alpha=0.6)
    ax1.set_xlabel('X (m)')
    ax1.set_ylabel('Y (m)')
    ax1.set_zlabel('Z (m)')
    ax1.set_title('Head Trajectory')
    ax1.legend()
    
    # Left hand trajectory
    ax2 = fig.add_subplot(132, projection='3d')
    ax2.plot(df['LeftHandPosX'], df['LeftHandPosY'], df['LeftHandPosZ'], 
             label='Left Hand', color='green', alpha=0.6)
    ax2.set_xlabel('X (m)')
    ax2.set_ylabel('Y (m)')
    ax2.set_zlabel('Z (m)')
    ax2.set_title('Left Hand Trajectory')
    ax2.legend()
    
    # Right hand trajectory
    ax3 = fig.add_subplot(133, projection='3d')
    ax3.plot(df['RightHandPosX'], df['RightHandPosY'], df['RightHandPosZ'], 
             label='Right Hand', color='red', alpha=0.6)
    ax3.set_xlabel('X (m)')
    ax3.set_ylabel('Y (m)')
    ax3.set_zlabel('Z (m)')
    ax3.set_title('Right Hand Trajectory')
    ax3.legend()
    
    plt.tight_layout()
    output_file = output_dir / 'trajectories_3d.png'
    plt.savefig(output_file, dpi=300)
    print(f"✓ Saved 3D trajectories to {output_file}")
    plt.close()

def plot_position_over_time(df, output_dir):
    """
    Plot position components over time
    
    Args:
        df (pandas.DataFrame): VR data
        output_dir (Path): Directory to save plots
    """
    time_seconds = df['Timestamp'] / 1000.0
    
    fig, axes = plt.subplots(3, 3, figsize=(15, 12))
    
    objects = ['Head', 'LeftHand', 'RightHand']
    colors = ['blue', 'green', 'red']
    
    for i, (obj, color) in enumerate(zip(objects, colors)):
        # X position
        axes[i, 0].plot(time_seconds, df[f'{obj}PosX'], color=color, alpha=0.7)
        axes[i, 0].set_ylabel(f'{obj} X (m)')
        axes[i, 0].grid(True, alpha=0.3)
        
        # Y position
        axes[i, 1].plot(time_seconds, df[f'{obj}PosY'], color=color, alpha=0.7)
        axes[i, 1].set_ylabel(f'{obj} Y (m)')
        axes[i, 1].grid(True, alpha=0.3)
        
        # Z position
        axes[i, 2].plot(time_seconds, df[f'{obj}PosZ'], color=color, alpha=0.7)
        axes[i, 2].set_ylabel(f'{obj} Z (m)')
        axes[i, 2].grid(True, alpha=0.3)
    
    for ax in axes[2, :]:
        ax.set_xlabel('Time (s)')
    
    plt.suptitle('Position Components Over Time', fontsize=16)
    plt.tight_layout()
    output_file = output_dir / 'position_time_series.png'
    plt.savefig(output_file, dpi=300)
    print(f"✓ Saved position time series to {output_file}")
    plt.close()

def plot_velocity_over_time(df, output_dir):
    """
    Plot velocity magnitude over time
    
    Args:
        df (pandas.DataFrame): VR data
        output_dir (Path): Directory to save plots
    """
    time_seconds = df['Timestamp'].values / 1000.0
    
    fig, axes = plt.subplots(3, 1, figsize=(12, 10))
    
    objects = ['Head', 'LeftHand', 'RightHand']
    colors = ['blue', 'green', 'red']
    
    for i, (obj, color) in enumerate(zip(objects, colors)):
        # Calculate velocity magnitude
        x = df[f'{obj}PosX'].values
        y = df[f'{obj}PosY'].values
        z = df[f'{obj}PosZ'].values
        
        dx = np.diff(x)
        dy = np.diff(y)
        dz = np.diff(z)
        dt = np.diff(time_seconds)
        
        velocity = np.sqrt(dx**2 + dy**2 + dz**2) / dt
        
        axes[i].plot(time_seconds[1:], velocity, color=color, alpha=0.7)
        axes[i].set_ylabel(f'{obj} Velocity (m/s)')
        axes[i].set_xlabel('Time (s)')
        axes[i].grid(True, alpha=0.3)
        axes[i].set_title(f'{obj} Velocity')
    
    plt.tight_layout()
    output_file = output_dir / 'velocity_time_series.png'
    plt.savefig(output_file, dpi=300)
    print(f"✓ Saved velocity time series to {output_file}")
    plt.close()

def generate_statistics(df, output_dir):
    """
    Generate and save statistical summary
    
    Args:
        df (pandas.DataFrame): VR data
        output_dir (Path): Directory to save report
    """
    stats = {}
    
    # Calculate statistics for each tracked object
    for obj in ['Head', 'LeftHand', 'RightHand']:
        distance = calculate_distance(df, obj)
        mean_vel, max_vel = calculate_velocity(df, obj)
        
        stats[obj] = {
            'Total Distance': f"{distance:.3f} m",
            'Mean Velocity': f"{mean_vel:.3f} m/s",
            'Max Velocity': f"{max_vel:.3f} m/s"
        }
    
    # Session statistics
    duration = df['Timestamp'].max() / 1000.0
    stats['Session'] = {
        'Duration': f"{duration:.2f} s",
        'Total Records': len(df),
        'Sampling Rate': f"{len(df) / duration:.1f} Hz"
    }
    
    # Print statistics
    print("\n" + "="*50)
    print("VR TRACKING STATISTICS")
    print("="*50)
    
    for category, metrics in stats.items():
        print(f"\n{category}:")
        for metric, value in metrics.items():
            print(f"  {metric:20s}: {value}")
    
    # Save to file
    output_file = output_dir / 'statistics.txt'
    with open(output_file, 'w') as f:
        f.write("="*50 + "\n")
        f.write("VR TRACKING STATISTICS\n")
        f.write("="*50 + "\n\n")
        
        for category, metrics in stats.items():
            f.write(f"{category}:\n")
            for metric, value in metrics.items():
                f.write(f"  {metric:20s}: {value}\n")
            f.write("\n")
    
    print(f"\n✓ Saved statistics to {output_file}")

def main():
    """Main analysis function"""
    print("\n" + "="*50)
    print("AIMLAB VR Data Analysis")
    print("Author: Pi Ko (pi.ko@nyu.edu)")
    print("="*50 + "\n")
    
    # Check command line arguments
    if len(sys.argv) != 2:
        print("Usage: python analyze_vr_data.py <filename.csv>")
        print("Example: python analyze_vr_data.py session_01.csv")
        sys.exit(1)
    
    # Load data
    filename = sys.argv[1]
    if not Path(filename).exists():
        print(f"✗ File not found: {filename}")
        sys.exit(1)
    
    df = load_vr_data(filename)
    
    # Create output directory
    output_dir = Path(filename).stem + "_analysis"
    output_dir = Path(output_dir)
    output_dir.mkdir(exist_ok=True)
    print(f"\n✓ Created output directory: {output_dir}")
    
    # Generate visualizations
    print("\nGenerating visualizations...")
    plot_trajectories(df, output_dir)
    plot_position_over_time(df, output_dir)
    plot_velocity_over_time(df, output_dir)
    
    # Generate statistics
    generate_statistics(df, output_dir)
    
    print("\n" + "="*50)
    print("Analysis complete!")
    print(f"Results saved to: {output_dir}")
    print("="*50 + "\n")

if __name__ == "__main__":
    main()

