# SPACEKING
2D Multiplayer online game made for Online Games subject for the Degree of Videogames Design Development at CITM (UPC University).
Made by Lucho Suaya (@lucho1) and Sergi Parra (@t3m1x)


## Description
Spaceking is a spaceships 2D Shooter Online multiplayer game in which the objective is to be the first in the ranking of deadliest spaceships and become the Space King!

As more spaceships you destroy, more points you'll get, and you will climb positions in the ranking, but be careful! If you get destroyed, you will lose all your points :( ...
... But you will respawn to get yourself again in the game!


## Controls
You can control your ship with the gamepad, by moving the left joystick to rotate, pressing the A to accelerate and the X to shoot.
You can also control it with the keyboard, with WASD to rotate, down-arrow to accelerate and left-arrow to shoot.


## Game Features
- To enter a server, specify its public address and port through which to enter (you'll need port forwarding!)
- To host a server, just type the port from which clients can connect and a name you want to use for it (max supported users 20)
- Users enter with the names and the spaceship type they want
- The users' join/leave is handled by the server, by adding them or deleting them from the ranking

- Each player will have a score and will have to climb positions in a ranking (formed by the scores of all players) shown in the UI

### Online Techniques Implemented
- WORLD STATE REPLICATION - by Sergi, Completed (NO KNOWN BUGS) - Each update of the world in the server (update, creation, destroy... of game objects) gets sent to all the clients so they can replicate this update on their system.

- REDUNDANCY - by Lucho, Completed (NO KNOWN BUGS) - To reduce packets loss, the server sends sends notifications to the clients on the next replication packet, for the inputs data packets received so the clients can send these packets until knowing which was the last processed by server.

- DELIVERY MANAGER - by Lucho with help from Sergi, Completed (NO KNOWN BUGS) - Delivery Notification system for the Server to store the Deliveries sent to client and the client to pass, on each PING, a list with the sequence numbers processed of the mentioned deliveries, so the server can re-send any delivery that failed to arrive (or discard it)

- CLIENT SIDE PREDICTION - by Lucho and Sergi together, Completed (NO KNOWN BUGS) - Client now predicts the players input to make the input feel more instantaneous (by applying it when sending it to the server). The world state replication acts as a kind of server reconciliation, so it matches the current input states for the players (in this case, the position of the spaceship) both in the client and the server.

- ENTITY INTERPOLATION - by Lucho and Sergi together, Completed (WITH BUGS) - To balance uncontinued updates from the server, each client try to smooth the other players' inputs so the player feels like the objects controlled by them are continuously updated (by interpolating, for instance, between two positions of the same spaceship). This feature is bugged, so it's deactivated by default, but it can be activated on each client for testing purposes. The only bug that it has is that it desynchronizes the positions of the spaceships between the server and the client when active, and we think that this happens because of the way in which we apply the interpolation and how we set the values for it, but we didn't had time to fix it.


Please, note that some features implemented during development were made by the two authors in a discord call and uploaded by one of them, specially the last ones and the game ranking mechanic.