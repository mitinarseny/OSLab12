# pps
Show active processes PIDs.

## C

### Build
```
cmake .
make
```

### Usage
```
./pps
```
To quit type `q` and press `<Enter>`.
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
