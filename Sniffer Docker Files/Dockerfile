# Use an official Python runtime as a parent image
#FROM python:2.7-slim
FROM gcc:latest

# Set the working directory to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

#install gcc compiler
RUN apt-get update && apt-get install -y \ 
	python \
	python3-pip

# Install any needed packages specified in requirements.txt
RUN pip3 install --trusted-host pypi.python.org -r requirements.txt

# Run app.py when the container launches
#CMD ["./runall.sh"]
ENTRYPOINT ["python3", "runall.py"]

