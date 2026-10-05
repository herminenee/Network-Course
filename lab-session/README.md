# ns-3 lab session

## Files

- `src/lab1-part1.cc`: multiple point-to-point clients connected to one server.
- `src/lab1-part2-original.cc`: original point-to-point and Ethernet network.
- `src/lab1-part2.cc`: Ethernet network extended with another point-to-point link and a new server.
- `src/lab1-part3.cc`: two Wi-Fi networks connected by a point-to-point link.
- `analysis/plot_part2_rtt.py`: compares original and modified Ethernet RTTs.
- `analysis/plot_part3_rtt.py`: plots the two-Wi-Fi-network RTTs.
- `results/`: captured ten-packet logs from the lab session.

All four C++ files were copied unchanged from `ns3-lab-code.tar.gz`, uploaded from the Ubuntu VM. The archive contains the final sources for all three lab parts.

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

## Run the lab programs

From this repository, copy all four sources into your existing ns-3 workspace:

```bash
cp lab-session/src/*.cc ~/ns-3.48/scratch/
cd ~/ns-3.48
```

### Part 1: multiple clients

`nClients` and `nPackets` both accept 1-5 and default to 1. Clients use separate 5 Mbps, 2 ms links to a shared server on UDP port 15. Each client starts at a uniform random time between 2 and 7 seconds; the server starts at 1 second, and the simulation stops at 20 seconds.

```bash
./ns3 run "scratch/lab1-part1 --nClients=5 --nPackets=4" 2>&1 | tee lab1-part1-output.txt
```

Expected replies: 20 total, four from each of five clients.

### Part 2: original and modified Ethernet networks

`nCsma` counts extra CSMA nodes beyond the gateway, so `--nCsma=4` creates five CSMA nodes. It defaults to 3, and a value of 0 is changed to 1. `nPackets` accepts 1-20 and defaults to 1. The Ethernet link is 100 Mbps with a 6560 ns delay; each point-to-point link is 5 Mbps with a 2 ms delay. The modified network adds a second point-to-point link from the last CSMA node to a new server.

Both programs use UDP port 9, 1024-byte payloads, and a 1-second interval. The server starts at 1 second and the client at 2 seconds; both applications stop at 25 seconds. Packet capture is enabled in both programs, using the `second` filename prefix. Move captures before running the other variant if you want to retain both sets.

```bash
./ns3 run "scratch/lab1-part2-original --nCsma=4 --nPackets=10" 2>&1 | tee lab1-part2-original-10.txt
./ns3 run "scratch/lab1-part2 --nCsma=4 --nPackets=10" 2>&1 | tee lab1-part2-modified-10.txt
```

Expected replies: 10 in each log. Use `--nPackets=20` for the twenty-packet checks.

### Part 3: two Wi-Fi networks

From your ns-3 workspace:

```bash
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

The uploaded final Part 3 source matches the previously published corrected source byte for byte. All four lab sources set the seed to `1234`; the included logs are lab results, not Homework 2 logs.

Earlier Ubuntu VM runs also verified 20 replies for Part 1 with five clients and four packets per client, and successful ten- and twenty-packet runs for both Part 2 variants. The Part 1 output log is not included in this repository.

Both plotting scripts were rerun successfully against the included logs when preparing this repository. C++ compilation was performed on the user's Ubuntu VM, not in the upload workspace.

## Attribution

The C++ programs adapt the ns-3 tutorial `first.cc`, `second.cc`, and `third.cc` examples and retain their `GPL-2.0-only` SPDX identifiers. See the [ns-3 tutorial](https://www.nsnam.org/docs/tutorial/html/) for the original examples.
