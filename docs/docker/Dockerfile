FROM alpine:3.9.4
MAINTAINER Manu Sanchez <Manu343726@gmail.com>

# Install python 3.6
RUN apk add --no-cache python3
RUN python3 -m ensurepip && \
    rm -r /usr/lib/python*/ensurepip
RUN pip3 install --upgrade setuptools
RUN if [ ! -e /usr/bin/pip ]; then ln -s pip3 /usr/bin/pip ; fi && \
    if [[ ! -e /usr/bin/python ]]; then ln -sf /usr/bin/python3 /usr/bin/python; fi
RUN rm -r /root/.cache

# Install doxygen 1.8.15 (As of 21/05/2019, see https://pkgs.alpinelinux.org/package/edge/main/x86/doxygen)
RUN apk add --no-cache doxygen

# Install python dependencies
RUN pip3 install --upgrade jinja2 Pygments

# Clone docs theme
RUN apk add --no-cache git
WORKDIR /
RUN git clone git://github.com/mosra/m.css --depth=1 && cd m.css/documentation/
ENV MCSS_DOCUMENTATION_DIR=/m.css/documentation

# Configure entrypoint
ENTRYPOINT ["/m.css/documentation/doxygen.py"]
