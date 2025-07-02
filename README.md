# Clustor - A Tor Hidden Service Load Balancer

This project provides a C++ application, built with the [nodepp](https://github.com/NodeppOfficial/nodepp) library, designed to create and manage multiple **Tor hidden services** and distribute incoming traffic among them for improved performance and resilience.

---

## Overview

The core idea behind this project is to leverage Tor's anonymity features while ensuring that your hidden service can handle a higher volume of requests by distributing the load across several independent Tor instances. This is achieved by spawning multiple worker processes, each running its own Tor hidden service, and then using HTTP redirects to direct clients to these individual services.

---

## Features

* **Distributed Tor Hidden Services**: Creates multiple Tor instances to handle requests.
* **CPU-Core Allocation**: Spawns worker processes based on available CPU cores for efficient resource utilization.
* **Automatic Tor Configuration**: Dynamically generates `manifest.txt` files for each Tor instance, handling port assignments and data directories.
* **HTTP Redirection**: Uses HTTP 302 redirects to guide clients to specific worker hidden services.
* **Process Resilience**: Includes basic respawn logic for worker processes to ensure continued operation.
* **Environment Variable Configuration**: Easily configure ports and directories via a `.env` file.