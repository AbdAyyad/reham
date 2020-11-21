FROM gcc:latest
COPY . /src
WORKDIR /src
RUN gcc main.c -o server
RUN chmod +x server
EXPOSE 8000-65000
CMD ["./server","8000"]
