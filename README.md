# Computer Networks Assignments Repository

Welcome to the **Computer Networks Assignments** repository. This repository contains materials, code, and resources for assignments completed as part of a Computer Networks course.

## Repository Structure

The repository is organized into the following sections:

### Assignment 1: Basic Networking Concepts
- **Files:**
  - `Assignment-1.pdf`: Assignment details.
  - `Question.pdf`: Problem statement.
  - `Screenshots/`: Contains output screenshots for various tasks.

### Assignment 2: Socket Programming
- **Files:**
  - `2024-PA02-Socket programming.pdf`: Assignment brief.
  - `Q1/`: Contains implementation of basic client-server socket communication.
    - `client.c`: Client-side code.
    - `server.c`: Server-side code.
    - `Makefile`: Compilation script.
  - `Q2/`: Contains advanced socket programming with multiple scenarios.
    - `single_thread_server.c`, `single_thread_client.c`: Single-threaded server and client.
    - `concurrent_server.c`, `concurrent_client.c`: Concurrent server and client.
    - `select_server.c`: Server implementation using `select()`.

### Assignment 3: Firewall and Packet Filtering
- **Files:**
  - `2022322-Assignment3.pdf`: Overview document.
  - `2024-PA03-iptables.pdf`: Instructions for working with iptables.

### Assignment 4: Network Simulation
- **Files:**
  - `2024-PA04-ns3.pdf`: Assignment details for network simulation using ns-3.
  - `Q1/`, `Q2/`, `Q3/`: Implementations for various simulation scenarios.
    - `*.cc`, `*.py`: Simulation scripts.
    - `*.csv`: Data outputs.
    - `*.png`: Graphical outputs.
    - `*.gp`: Gnuplot scripts.
  - `Report_A4_2022322.pdf`: Final report.

## Usage Instructions

1. Clone the repository:
   ```bash
   git clone <repository-url>
   ```

2. Navigate to the assignment folder of interest:
   ```bash
   cd CN-Assignments/Assignment-<number>
   ```

3. For programming tasks:
   - Compile using the provided `Makefile` (if applicable):
     ```bash
     make
     ```
   - Execute the programs as per the instructions in the respective assignment PDFs.

4. For simulation tasks:
   - Use the scripts and data files to reproduce results.
   - View plots using Gnuplot or similar tools.

## Prerequisites

- **Programming:** Knowledge of C, Python, and bash scripting.
- **Tools:**
  - GCC/G++ for C programs.
  - Python 3.x.
  - ns-3 simulator for network tasks.
  - Gnuplot for plotting graphs.

## Screenshots and Outputs

Screenshots and output images are included in their respective folders for reference.

## License

This repository is intended for educational purposes. Feel free to use the resources with proper attribution.

## Acknowledgments

Special thanks to the course instructors and teaching assistants for their guidance and support.
