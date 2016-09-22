#include <zmq.hpp>
#include "x264decoder.h"
#include <opencv2/opencv.hpp>

#define RGB_SIZE  640*480*3

int width = 640;
int height = 480;

struct DataPacked {
	uint32_t frame_id_;
	uint32_t color_size_;
    char     data_[RGB_SIZE];
}__attribute__((packed));

int main(int argc, char **argv){

	/* Starting Connection */
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);
    std::cout << "Connecting to kinect server..." << std::endl;
    std::string connection_address("tcp://127.0.0.1:5555");
	socket.connect (connection_address.c_str());

    DataPacked *data;
	x264Decoder x264_decoder;
	x264_decoder.initialize(width, height);

    cv::Mat image(480, 640, CV_8UC3);

	while(true){

		// Send request to server 
		zmq::message_t request (6);
		memcpy ((void *) request.data (), "Hello", 5);
		socket.send (request);
		//Receive frame from server 
		zmq::message_t frame_camera;
        socket.recv(&frame_camera);
        data = (DataPacked *)frame_camera.data();
        x264_decoder.decodeFrame(&(data->data_[0]), data->color_size_, image.data);
        cv::imshow("image", image);
        cv::waitKey(10);

	}
}
