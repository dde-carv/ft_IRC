*This project has been created as part of the 42 curriculum by dicarval, ricmanue.*

# ft_IRC

## Description

**ft_IRC** is a custom implementation of an Internet Relay Chat (IRC) server, written in C++98. The goal of this project is to understand the underlying protocols of real-time communication, socket programming, and non-blocking I/O operations.

This server is capable of handling multiple clients simultaneously, managing channels, and processing standard IRC commands. It adheres to the fundamental principles of the IRC protocol, allowing standard IRC clients (like HexChat or netcat) to connect and interact with it.

## Instructions

### Compilation

To compile the project, clone the repository and run `make` at the root directory:

```bash
git clone https://github.com/dde-carv/ft_IRC.git
cd ft_IRC
make
```

This will generate an executable named `ircserv`.

### Execution

To start the server, run the executable with the listening port and a connection password:

```bash
./ircserv <port> <password>
```

**Arguments:**
- `<port>`: The port number on which the server will listen for incoming connections (e.g., `6697`).
- `<password>`: The password required for clients to authenticate.

**Example:**
```bash
./ircserv 6697 mysecretpassword
```

### Connecting as a Client

You can connect to the server using any IRC client or `netcat`.

**Using netcat (nc):**
```bash
nc localhost 6697
PASS mysecretpassword
NICK mynickname
USER myusername 0 * :Real Name
```

## Features & Commands

The server supports the following IRC commands:

- **Authentication**: `PASS`, `NICK`, `USER`
- **Channel Operations**: `JOIN`, `PART`, `TOPIC`, `INVITE`, `KICK`, `MODE`
- **Communication**: `PRIVMSG`
- **Server**: `QUIT`

#### PASS

> The `PASS` command is used to authenticate the client via password. If you typed in the password in the `Hexchat` Network tab, the client will send this password to the server.
>
> In our case, we chose to deal with authentication through a boolean flag. If the password is correct, the flag is set to true. Only if the client is also registered (via `NICK` and `USER`), the server responds with the `001` reply, welcoming the client to the server.
>
> In case the password is incorrect, the server responds with the `464` reply (ERR_INCORPASS), informing the client that the password is incorrect. The client must provide the correct password to proceed with registration.

#### NICK

> The `NICK` command is used to set the client's nickname. Registration is not considered complete until a valid nickname is set. The client can change their nickname at any time by sending the `NICK` command again. However, there cannot exist a client with no nick, nor a client with the same nick as another active client.

#### USER

> The `USER` command is used to set the client's username and realname. The `USER` command is mandatory, and it is good practice to send it after the `NICK` command. The `USER` command is used to finalize the registration of the client with the server.
>
> In our case, we chose to only consider a client registered if the `username`, `realname`, and `nickname` are all set. However, due to particularities of the `HexChat` client, the `NICK` command might fail before the `USER` command is sent (e.g., if the nickname is taken). It is up to the client to retry with a valid nickname to complete registration.

#### JOIN

> The `JOIN` command is used to join a channel. The client sends the `JOIN` command to the server, and the server responds with the appropriate replies. `HexChat` uses the server's reply to the join command to perform actions in the Graphical User Interface (such as opening a new tab).
>
> The `JOIN` command is a Client->Channel command, so the server sends a confirmation reply to all users currently in the channel.
>
> We also handle the `JOIN` command to create channels that do not exist. If a client joins a channel that has not been created yet, the server creates it on the fly and assigns the creating user as the **Channel Operator**. This allows for dynamic channel creation and flexibility.

#### PRIVMSG

> The `PRIVMSG` command is used to send a message to a user or a channel. The client sends the `PRIVMSG` command to the server, and the server relays the message to the target. `HexChat` uses the server's reply (or the echoed message) to display the conversation in the appropriate tab.

#### QUIT

> The `QUIT` command is used to disconnect the client from the server. The client sends the `QUIT` command to the server, and the server closes the connection and notifies other users in shared channels. In `HexChat`, this disconnects the client from the server gracefully.

#### MODE

> The `MODE` command is used to see or change a channel's modes. This was one of the most challenging commands to implement due to the complexity of parsing different modes and maintaining modularity.
>
> Channel operators can change the channel's modes. There are five modes we handle:
> - `+o`/`-o`: Give/remove operator status to/from a user.
> - `+t`/`-t`: Allow/prevent users who are not operators from changing the topic.
> - `+i`/`-i`: Set/unset the channel as invite-only.
> - `+k`/`-k`: Set/unset a password for the channel.
> - `+l`/`-l`: Set/unset the channel's user limit.

#### KICK

> The `KICK` command is used to kick a user from a channel. The client sends the `KICK` command to the server, and the server notifies the channel members. To kick someone from a channel, the client must be a **Channel Operator**.

#### INVITE

> The `INVITE` command is used to invite a user to a channel. To invite someone, the client must be a member of that channel.
>
> If the channel is set to **Invite-Only mode (+i)**, only **Channel Operators** can issue invites. `HexChat` abstracts the process of accepting the invitation and joining the channel, often allowing users to simply click the invitation message.

#### TOPIC

> The `TOPIC` command is used to set or view the channel's topic.
>
> If the channel has the **Topic Restriction mode (+t)** enabled, only **Channel Operators** can change the topic. Otherwise, any user in the channel can update it. This is visually represented in `HexChat`, which provides a dedicated space for the channel's topic.

## Resources

Here are some of the classic references and documentation used to build this project:

- **RFC 1459**: [Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459) - The original specification.
- **ircdocs.horse**: [Modern IRC Client Protocol](https://modern.ircdocs.horse/) - Overview and reference of the IRC client protocol as it is implemented today.
- **Youtube**: [99% of Developers Don't Get Sockets](https://www.youtube.com/watch?v=D26sUZ6DHNQ)

### AI Usage

> "AI tools were used to assist in debugging logic and to clarify the expected behavior of specific IRC commands based on the RFCs."
---
