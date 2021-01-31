from uwimg import *
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-box7")

im = load_image("data/dog.jpg")
f = make_highpass_filter()
highpass = convolve_image(im, f, 0)
clamp_image(highpass)
save_image(highpass, "dog-highpass")

im = load_image("data/dog.jpg")
f = make_emboss_filter()
emboss = convolve_image(im, f, 1)
clamp_image(emboss)
save_image(emboss, "dog-emboss")

im = load_image("data/dog.jpg")
f = make_sharpen_filter()
sharpen = convolve_image(im, f, 1)
clamp_image(sharpen)
save_image(sharpen, "dog-sharpen")

im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
gaussian = convolve_image(im, f, 1)
save_image(gaussian, "dog-gaussian")

im = load_image("data/dog.jpg")
res = sobel_image(im)
mag = res[0]
feature_normalize(mag)
save_image(mag, "magnitude")

im = load_image("data/dog.jpg")
res = colorize_sobel(im)
save_image(res, "colorize-sobel")