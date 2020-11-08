# TiesOfNetworks
Repository for the exercises of the Networks &amp; Online Games subject for the Degree of Videogames Design &amp; Develpment at CITM (UPC University)


Made by Lucho Suaya (@lucho1) and Sergi Parra (@t3m1x)

# -- MULTI-USER CHAT APPLICATION --
## Features
- Users enter with their names and get an ID assigned
- They can change the text color
- If they are forcely disconnected, server will warn. If they are normally disconnected (through button), server will just notice

- To enter a server, specify its public address and port through which to enter (you'll need port forwarding!)
- To host a server, just type the port from which clients can connect and a name you want to use for it (max supported users 100)

## Chat Commands & Usability
In the client side, you will be able to click/right-click a user to send a whisper. To close the popup window that will show up, just double-click anywhere.

Try using one of our chat commands:
    /help or /h       - Displays help on command(s), syntax is "/help (command)"
    /whisper or /w    - Message someone privately, syntax is "/whisper \[username] \[message]"
    /logout           - Disconnects from chat
    /nick             - Changes your name in the chat, syntax is "/nick \[new_username]"
    /kick or /k       - Kicks user out of server, syntax is "/kick \[username]"
    
Game commands:
    /ksm 
         start                   - Start Kill Sex Marry Game
         stop                    - Stop the KSM Game
         kill or k \[username]   - Kill username
         sex or s \[username]    - Sex username
         marry or m \[username]  - Marry username
         help                    - Info on game 
    /rr 
        start                    - Start Roussian Roulette Game
        stop                     - Stop Roussian Roulette Game
        bullet \[slot number]    - Load bullet into slot number
        shoot                    - Shoot the gun, try your luck
        help                     - Info on game 

    /unscramble 
        start                    - Start Word Unscramble Game
        stop                     - Stop Word Unscramble Game
        word \[word]             - Send the new word
        help                     - Info on game 
    /chained 
        start                    - Start Chained Word Game
        stop                     - Stop Chained Word Game
        word \[word]             - Send the new word
        help                     - Info on game 