FROM gcc:latest
COPY . /src
WORKDIR /src
RUN gcc main.c -o client
RUN chmod +x client
EXPOSE 10000-65000
CMD ["./client","172.17.0.1" , "8000"]

