string = """28-balloon
16-euro
1-copyright
11-six
24-pitchfork
11-six
13-at
28-balloon
8-pumpkin
21-paragraph
4-smileyface
16-euro
30-upsidedowny
23-leftc
26-cursive
31-bt
31-bt
27-tracks
12-squigglyn
26-cursive
5-doublek
7-squidknife
22-rightc
14-ae
7-squidknife
3-hollowstar
15-meltedthree
5-doublek
21-paragraph
24-pitchfork
9-hookn
9-hookn
30-upsidedowny
20-questionmark
19-dragon
18-nwithhat
23-leftc
20-questionmark
3-hollowstar
4-smileyface
2-filledstar
6-omega"""

string = string.split('\n')

newstring = [string[i::6] for i in range(6)]
newstring = '\n\n'.join(['\n'.join(string) for string in newstring])

print(newstring)