# pps
Show active processes PIDs.

## Build

```bash
cd c && cmake . && make
```

## Usage

```bash
# For C
./c/pps

# For shell
./sh/pps.sh
```

To quit, enter anything.
To show active terminals send `SIGINT` (press `<Ctrl+C>`) 4 times.


## Docker

### Build

```bash
docker build -t pps_ubuntu .
```

### Run

```bash
docker run --rm -it pps_ubuntu
```
