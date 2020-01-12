FROM ubuntu:18.04

RUN yes | unminimize

RUN apt-get update && \
  apt-get install --yes --no-install-recommends \
    manpages-dev

ENTRYPOINT ["bash"]
