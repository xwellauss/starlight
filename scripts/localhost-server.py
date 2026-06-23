from http.server import SimpleHTTPRequestHandler, HTTPServer
import ssl

class MyHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory="..", **kwargs)

    def end_headers(self):
        # Set headers for cross-origin isolation
        # self.send_header('Cross-Origin-Embedder-Policy', 'credentialless')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        super().end_headers()

# Define the server address and port
HOST = '0.0.0.0'
PORT = 8000

httpd = HTTPServer((HOST, PORT), MyHandler)

context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
context.load_cert_chain(certfile='./0.0.0.0.pem', keyfile='./0.0.0.0-key.pem')
httpd.socket = context.wrap_socket(httpd.socket, server_side=True)

print(f"Serving HTTPS on https://{HOST}:{PORT}")
httpd.serve_forever()
