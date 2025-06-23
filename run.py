import os

if os.path.exists("main"):
    os.remove("main")
files = ['main.cpp', 'helpers.cpp', 'glad.c', 'generator.cpp']
for file in files:
    files[files.index(file)] = file
os.system(f"g++ -fPIC {' '.join(files)} -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -o main")
# os.system("prime-run ./main")
os.system("./main")