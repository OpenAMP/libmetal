FROM ubuntu:24.04

ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

# Install prerequisites
RUN sed -i \
        -e 's|http://archive.ubuntu.com/ubuntu/|http://azure.archive.ubuntu.com/ubuntu/|g' \
        -e 's|http://security.ubuntu.com/ubuntu/|http://azure.archive.ubuntu.com/ubuntu/|g' \
        /etc/apt/sources.list.d/ubuntu.sources \
    && apt-get --quiet=2 -o Acquire::Retries=5 update --error-on=any \
    && apt-get install --quiet=2 --assume-yes \
        ca-certificates \
        sudo \
        git \
        python3 \
        python3-pip \
        python3-venv \
        wget \
    && rm -rf /var/lib/apt/lists/*

# Copies your code file from your action repository to the filesystem path `/` of the container
COPY entrypoint.sh /entrypoint.sh

# Code file to execute when the docker container starts up (`entrypoint.sh`)
ENTRYPOINT ["/entrypoint.sh"]
