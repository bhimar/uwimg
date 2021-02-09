from uwimg import *

def draw_corners():
    im = load_image("data/Rainier1.png")
    detect_and_draw_corners(im, 2, 50, 3)
    save_image(im, "corners")

def draw_matches():
    a = load_image("data/Rainier1.png")
    b = load_image("data/Rainier2.png")
    m = find_and_draw_matches(a, b, 2, 50, 3)
    save_image(m, "matches")

def easy_panorama():
    im1 = load_image("data/Rainier1.png")
    im2 = load_image("data/Rainier2.png")
    pan = panorama_image(im1, im2, thresh=50)
    save_image(pan, "easy_panorama")

def rainier_panorama():
    im1 = load_image("data/Rainier1.png")
    im2 = load_image("data/Rainier2.png")
    im3 = load_image("data/Rainier3.png")
    im4 = load_image("data/Rainier4.png")
    im5 = load_image("data/Rainier5.png")
    im6 = load_image("data/Rainier6.png")
    pan = panorama_image(im1, im2, thresh=5)
    save_image(pan, "rainier_panorama_1")

    pan2 = panorama_image(pan, im5, thresh=5)
    save_image(pan2, "rainier_panorama_2")

    pan3 = panorama_image(pan2, im6, thresh=5)
    save_image(pan3, "rainier_panorama_3")

    pan4 = panorama_image(pan3, im3, thresh=5)
    save_image(pan4, "rainier_panorama_4")

    pan5 = panorama_image(pan4, im4, thresh=5)
    save_image(pan5, "rainier_panorama_5")

def sun_panorama():
    im1 = load_image("data/sun1.png")
    im2 = load_image("data/sun2.png")
    im3 = load_image("data/sun3.png")
    im4 = load_image("data/sun4.png")
    im5 = load_image("data/sun5.png")
    pan = panorama_image(im1, im2, thresh=5)
    save_image(pan, "sun_panorama_1")

    pan2 = panorama_image(pan, im3, thresh=5)
    save_image(pan2, "sun_panorama_2")

    pan3 = panorama_image(pan2, im4, thresh=5)
    save_image(pan3, "sun_panorama_3")

    pan4 = panorama_image(pan3, im5, thresh=5)
    save_image(pan4, "sun_panorama_4")

# draw_corners()
# draw_matches()
# easy_panorama()
# rainier_panorama()
sun_panorama()
# field_panorama()

