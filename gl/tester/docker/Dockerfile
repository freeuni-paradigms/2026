FROM ubuntu:latest

WORKDIR /app

RUN apt-get update
RUN apt-get -y install --no-install-recommends build-essential
RUN apt-get -y install --no-install-recommends unzip default-jre
RUN apt-get -y install python3

# RUN git clone https://github.com/giolekva/gl.git

ENV GL_LIB_PATH=/app/gl
ENV CC=gcc
ENV LD=gcc

ENV MODE='--class'

RUN apt-get -y install --no-install-recommends wget
RUN wget ftp://ftp.gnu.org/pub/gnu/kawa/kawa-3.1.1.zip
RUN apt-get -y install --no-install-recommends unzip
RUN unzip kawa-3.1.1.zip
RUN mv kawa-3.1.1 kawa
RUN rm kawa-3.1.1.zip

RUN mkdir /venus
COPY *.jar /venus

ENV PATH="${PATH}:/app/kawa/bin"

CMD  make -C /app/gl/lunit lunit \
     && make -C /app/gl/tester run_tests \
     && /app/gl/tester/run_tests $MODE --students_dir=/exam/students --problems_dir=/exam/problems --results_dir=/exam/results
