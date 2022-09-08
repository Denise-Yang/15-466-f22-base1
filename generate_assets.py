# generate map
# 
from PIL import Image

palette = {}

tiles = []
def generate_tiles():
    im = Image.open('assets/tiles.png').convert("RGB")
    im2 = Image.open('assets/walko.png').convert("RGB")
    tileW = 8
    tileH = 8
    #every odd index is bit1 
    bit0= [0] * tileH * tileW
    bit1 = [0] * tileH * tileW
    with open("tiles.txt", 'w') as f_out:
        for z in range(5):
            for j in range(tileH):
                for i in range(tileW):
                    if z==4:
                        pix = im2.getpixel((i, j))
                    else:
                        pix = im.getpixel((i, tileH*z + j))
                    color  = (pix[0], pix[1], pix[2])
                    if (color in palette.keys()):
                        color_idx = palette[color]
                        bit0[j*tileW + i] = color_idx&1
                        bit1[j*tileW + i] = (color_idx >> 1) & 1
        
                    else:
                        print("color DNE", end =" ")
                        print(pix)
            tiles.append([bit0.copy(), bit1.copy()])
            for j in range(tileH):
                for i in range(tileW):
                    f_out.write(str(bit0[j*tileW + i]))
            for j in range(tileH):
                for i in range(tileW):
                    f_out.write(str(bit1[j*tileW + i]))


def generate_maps():
    im = Image.open('assets/maze.png').convert("RGB")
    
    print(im.height)
    print("height")
    floorTile = 1
    spikeTile = 2
    bgTile = 0
    w = 64
    h = 60
    maze = [0] * w * h
    for j in range(0,h):
        for i in range(0,w):
            if (i >= 32):
                maze[j*w+i]  = 0
                continue

            if ( j >= 32):
                maze[j*w+i]  = 0
                continue
            color = im.getpixel((i,j))
            if (color[0] == 0xFF and color[1] == 0xFF and color[2] == 0xFF ):
                maze[j*w+i] = 1
            elif (color[0] == 0 and color[1] == 0 and color[2] == 0 ):
                maze[j*w+i] = 0
            elif color[0] == 0xFF:
                #danger 
                maze[j*w+i] = 2
            elif color[1] == 0xFF: 
                #end
                maze[j*w+i] = 3
            elif color[2] == 0xFF: 
                #start
                maze[j*w+i] = 1
    for j in range(0,60):
        for i in range(0,64):
            print(maze[j*w+i], end="" )
        print()
    with open("maps.txt", 'w') as f_out:
        for tile in maze:
            f_out.write(str(tile))


def formatInt(val):
    if val < 10:
        return "00" + str(val)
    elif val  <100:
        return "0"+str(val)
    else:
        return str(val)
def generate_palettes():
    im = Image.open('assets/palette.png').convert("RGB")
    # get pixels
    with open("palettes.txt", 'w') as f_out:
        color_idx = 0
        # f_out.write(str('pal0'))
        # f_out.write(str((8*3*4).to_bytes(4, byteorder='big', signed=False)))
        
        for j in range(im.height):
            for i in range(im.width):
                r = int(im.getpixel((i, j))[0])
                g = int(im.getpixel((i, j))[1])
                b = int(im.getpixel((i, j))[2])
                palette[(r,g,b)] = color_idx
                print((r,g,b))
                color = r<<24 | g << 16 | b << 8
              
                f_out.write(formatInt(r)+formatInt(g)+formatInt(b)+"\n")

                color_idx+=1

# or

def main():
   generate_palettes()
   generate_tiles()
   generate_maps()

if __name__=="__main__":
    main()