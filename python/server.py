import tornado.ioloop
import tornado.web
import tornado.websocket

acceptedEspDevices = ["esp8266"]  # Use with esp8266's client.addHeader code


class MyWebSocketHandler(tornado.websocket.WebSocketHandler):
    espDevices = {}

    def check_origin(self, origin):
        return True

    def open(self):
        print("A Device Connected")
        if (
            "Device" in self.request.headers
        ):  # Check if there is a header named "Device"
            if (
                self.request.headers["Device"] in acceptedEspDevices
            ):  # Check if the connected device is an ESP device
                self.espDevices[self.request.headers["Device"]] = (
                    self  # Add Dict to the connected device info
                )

    def on_message(self, message):
        message = message.encode("utf-8")
        print(f"Incoming message : {message}")
        for device in self.espDevices:
            self.espDevices[device].write_message(message)

    def on_close(self):
        print("A device disconnected")


class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("index.html")


def make_app():
    return tornado.web.Application(
        [
            (r"/", MainHandler),
            (r"/Esp", MyWebSocketHandler),
        ]
    )


if __name__ == "__main__":
    app = make_app()
    app.listen(8888)
    tornado.ioloop.IOLoop.current().start()
