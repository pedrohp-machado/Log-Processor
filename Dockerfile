# Build (Compilação)
FROM gcc:latest as builder
WORKDIR /app
COPY . .

# Usa o seu Makefile para compilar
RUN make

# Runtime (Imagem leve apenas para rodar)
# Isso reduz o tamanho da imagem de 1GB+ para uns 50MB
FROM debian:buster-slim
WORKDIR /app
COPY --from=builder /app/log_processor .
COPY --from=builder /app/generate_data.py .

# python minimal para gerar dados
RUN apt-get update && apt-get install -y python3

CMD ["./log_processor"]