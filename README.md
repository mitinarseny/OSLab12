# pps
Show active processes PIDs.

## C

### Build
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
To show active terminals send `SIGINT` 4 times (or press `<Ctrl+C>` 4 times).


## Docker

### Build

```bash
docker build -t pps_ubuntu .
```

### Run

```bash
docker run --rm -it pps_ubuntu
```
