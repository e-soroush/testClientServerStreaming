#include <zmq.hpp>
#include "x264encoder.h"
#include <opencv2/opencv.hpp>

#define RGB_SIZE 640*480*3

int width = 640;
int height = 480;

x264Encoder x264_encoder;

struct DataPacked{
	uint32_t frame_id_;
	uint32_t color_size_;
	char     data_[RGB_SIZE]; 
}__attribute__((packed));


//Pack the NALs created by x264 into a single packet.
unsigned int pack_rgb_data(DataPacked &data){
	unsigned int tmp_size = 0;
	
	x264_nal_t nal;
    while(x264_encoder.isNalsAvailableInOutputQueue()){
        nal = x264_encoder.getNalUnit();
	    memcpy(&(data.data_[tmp_size]), nal.p_payload, nal.i_payload);
		tmp_size += nal.i_payload;
	}
	data.color_size_ = tmp_size;
	//Size of DataPacked after data insert
	return sizeof(uint32_t) * 2 + data.color_size_;
}


int main(){

	DataPacked data;

    x264_encoder.initialize(width, height);

	zmq::context_t context (1);	
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind ("tcp://127.0.0.1:5555");
    cv::VideoCapture vc(0);
    cv::Mat image;
	while(true){
        //Any grabber to get raw rgb data from camera.
        vc.read(image);

		//Wait for the client to send a request.
        zmq::message_t request;
        socket.recv (&request);

		//Encode the frame with x264
        x264_encoder.encodeFrame(image.data, RGB_SIZE);
		unsigned int message_size = pack_rgb_data(data);

		//Send the data to the client
		zmq::message_t video_frame (message_size);
        memcpy ((void *) video_frame.data(), &data, message_size);
        socket.send (video_frame);
	}
}

