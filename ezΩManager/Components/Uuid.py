import uuid;
x = uuid.uuid4();
print(x);
with open("./temp.txt", "w", encoding = "UTF-8") as file:
	file.write(str(x));
pass
