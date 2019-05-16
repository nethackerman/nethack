import sys

ROOM = """
-----------
----...----
---.....---
...........
---.....---
----...----
-----------
"""

for r in range(2):
    for line in ROOM.split('\n'):
        for i in range(5):
            sys.stdout.write(line)
        sys.stdout.write('\n')

