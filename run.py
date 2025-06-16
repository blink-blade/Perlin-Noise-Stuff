import os

if os.path.exists("main"):
    os.remove("main")
os.system("g++ -fPIC main.cpp helpers.cpp glad.c generator.cpp -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -o main")
os.system("prime-run ./main")