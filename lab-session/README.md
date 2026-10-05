# ns-3 lab session

## Files

- `src/lab1-part3.cc`: two Wi-Fi networks connected by a point-to-point link.
- `analysis/plot_part2_rtt.py`: compares original and modified Ethernet RTTs.
- `analysis/plot_part3_rtt.py`: plots the two-Wi-Fi-network RTTs.
- `results/`: captured ten-packet logs from the lab session.

The final Part 1 source and both Part 2 sources have not yet been uploaded to this repository. This directory currently preserves the available Part 3 implementation and the analysis inputs for Parts 2 and 3.

## Part 3 configuration

| Setting | Value |
| --- | --- |
| ns-3 version used for the lab | 3.48 |
| Random seed | 1234 |
| `nWifi` | 1-9 STAs in each network; default 3 |
| `nPackets` | 1-20; default 1 |
| Wi-Fi networks | Independent Yans channels, separate SSIDs |
| Wi-Fi channels | 36 and 40, 20 MHz, 5 GHz |
| STA mobility | RandomWalk2d within x/y bounds -50 to 50 |
| AP mobility | Constant position |
| Point-to-point link | 5 Mbps, 2 ms |
| UDP echo | Port 9, 1024-byte payload, 1-second interval |
| Server/client start | 1 s / 2 s |
| Application and simulation stop | 25 s |

The client is the last STA in the first Wi-Fi network, and the server is the last STA in the second network. Global IPv4 routing connects the three subnets.

## Run Part 3

From this repository, copy the source into your existing ns-3 workspace:

```bash
cp lab-session/src/lab1-part3.cc ~/ns-3.48/scratch/
cd ~/ns-3.48
./ns3 run scratch/lab1-part3
./ns3 run "scratch/lab1-part3 --nWifi=4 --nPackets=20" 2>&1 | tee lab1-part3-20.txt
./ns3 run "scratch/lab1-part3 --nWifi=4 --nPackets=10" 2>&1 | tee lab1-part3-10.txt
```

Count the received replies:

```bash
grep -c "client received" lab1-part3-20.txt
grep -c "client received" lab1-part3-10.txt
```

Expected counts: 20 and 10, respectively.

## Reproduce the RTT plots

Return to this repository and install the plotting dependency in a Python virtual environment:

```bash
python3 -m venv .venv
source .venv/bin/activate
python3 -m pip install -r lab-session/analysis/requirements.txt
python3 lab-session/analysis/plot_part2_rtt.py lab-session/results/lab1-part2-original-10.txt lab-session/results/lab1-part2-modified-10.txt
python3 lab-session/analysis/plot_part3_rtt.py lab-session/results/lab1-part3-10.txt
```

Each script writes PNG and PDF figures beside itself. RTT is the difference between the client receive and send timestamps, expressed in milliseconds. The parsers require ten complete request/reply pairs and reject overlapping sends. Values are approximate because application logs round timestamps.

## Validation

The Part 3 lab source compiled and ran on the Ubuntu VirtualBox VM with ns-3.48. The default run received one reply; the four-STA-per-network runs received all 20 and all 10 replies. The uploaded ten-packet log gives RTTs of 21.45, 8.03, 8.96, 8.04, 8.04, 8.03, 8.03, 8.00, 8.00, and 8.00 ms.

The archived source includes the seven-line removal that fixed the unmatched brace in the earlier uploaded copy. Its executable code also matches the completed Homework 2 copy after restoring the lab seed from `123456789` to `1234`. The lab logs belong to seed `1234`; they are not the Homework 2 logs.

Both plotting scripts were rerun successfully against the included logs when preparing this repository. C++ compilation was performed on the user's Ubuntu VM, not in the upload workspace.

## Attribution

The C++ program adapts the ns-3 tutorial `third.cc` example and retains its `GPL-2.0-only` SPDX identifier. See the [ns-3 tutorial](https://www.nsnam.org/docs/tutorial/html/) for the original examples.
