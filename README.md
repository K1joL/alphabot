# AlphaBot
The alphabot project implements a request handler for Alphabot from this repository: https://github.com/MrQuaters/kos-abot.
The main idea is to allow the user to order a drink for delivery within the room.

# How it works
1. The user orders a drink.
2. The server determines the position of the user and the robot.
3. The server, using the opencv computer vision library and a camera, controls the robot that picks up and delivers drinks.

# The interaction occurs via the MQTT protocol.
```
+---------------+              +---------------+             +---------------+
|               |              |               |             |               |
|   kos_abot    |  <- MQTT ->  |    control    | <- MQTT ->  |  telegram_bot |
|               |              |               |             |               |
+---------------+              +---------------+             +---------------+
```
