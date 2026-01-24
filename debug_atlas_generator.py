from PIL import Image, ImageDraw

IMG_SIZE = 512
TILE_SIZE = 16
GRID = 32

BLOCK_W = 2
BLOCK_H = 3
SPACING = 1

# bit layout per row
ROWS = [4, 4, 2]

img = Image.new("RGB", (IMG_SIZE, IMG_SIZE), "white")
draw = ImageDraw.Draw(img)

def draw_tile(x0, y0, index):
    # tile border
    draw.rectangle([x0, y0, x0 + TILE_SIZE - 1, y0 + TILE_SIZE - 1], outline="black", fill="white")

    bits = format(index, "010b")
    bit_i = 0

    start_x = x0 + 2
    start_y = y0 + 2

    y = start_y
    for row_bits in ROWS:
        x = start_x
        for _ in range(row_bits):
            color = "black" if bits[bit_i] == "1" else "grey"
            draw.rectangle([x, y, x + BLOCK_W - 1, y + BLOCK_H - 1], fill=color, outline=None)
            x += BLOCK_W + SPACING
            bit_i += 1
        y += BLOCK_H + SPACING

for idx in range(1024):
    tx = idx % GRID
    ty = idx // GRID
    draw_tile(tx * TILE_SIZE, ty * TILE_SIZE, idx)

img.save("binary_tile_grid.png")
