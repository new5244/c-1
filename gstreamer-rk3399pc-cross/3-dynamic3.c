#include <gst/gst.h>

/* structure to contain all our information, so we can pass it to callbacks */

typedef struct _CustomData {
	GstElement *pipeline;
	GstElement *source;
	GstElement *convert;
	GstElement *sink;

	//GstElement *tee;
	GstElement *audio_queue;
	GstElement *video_queue;
	GstElement *video_convert;
	GstElement *video_sink;

} CustomData;


/* handler for the pad-added signal */
static void pad_added_handler(GstElement *src, GstPad *pad, CustomData *data);

int main(int argc, char *argv[])
{
	CustomData data;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;
	gboolean terminate = FALSE;

	//GstPad *tee_audio_pad, *tee_video_pad;
	//GstPad *queue_audio_pad, * queue_video_pad;


	/* initialize gstreamer */
	gst_init(&argc, &argv);

	/* create the elements */
	data.source = gst_element_factory_make("uridecodebin", "source");
	data.convert = gst_element_factory_make("audioconvert", "convert");
	data.sink = gst_element_factory_make("autoaudiosink", "sink");

	//data.tee = gst_element_factory_make("tee", "tee");
	data.audio_queue = gst_element_factory_make("queue", "audio_queue");
	data.video_queue = gst_element_factory_make("queue", "video_queue");
	data.video_convert = gst_element_factory_make("videoconvert", "video_convert");
	data.video_sink = gst_element_factory_make("autovideosink", "video_sink");


	/* create the empty pipeline */
	data.pipeline = gst_pipeline_new("test-pipeline");

	if ( !data.pipeline || !data.source || !data.convert || !data.sink ) {
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	if ( !data.audio_queue || !data.video_queue || !data.video_convert || !data.video_sink) {
		g_printerr("Not all elements could be created. (2)\n");
		return -1;
	}


	/* build the pipeline. Note that we are Not linking the source at this point. We will do it later. */
	gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.convert, data.sink, data.audio_queue, data.video_queue, data.video_convert, data.video_sink, NULL);
	if ( !gst_element_link_many( data.audio_queue, data.convert, data.sink, NULL) ) 
	{
		g_printerr("Elements could be linked. audio convert-sink\n");
		gst_object_unref(data.pipeline);
		return -1;
	}

	if ( !gst_element_link_many( data.video_queue, data.video_convert, data.video_sink, NULL) ) 
	{
		g_printerr("Elements could be linked. video convert-sink\n");
		gst_object_unref(data.pipeline);
		return -1;
	}






	/* set the URI to play */
	g_object_set(data.source, "uri", "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", NULL);

	/* Connect to the pad-added signal */
	g_signal_connect(data.source, "pad-added", G_CALLBACK(pad_added_handler), &data);

	/* Start playing */
	ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
	if ( ret == GST_STATE_CHANGE_FAILURE ) {
		g_printerr("Unable to set the pipeline to the playing state.\n");
		gst_object_unref(data.pipeline);
		return -1;
	}


	/* Listen to the bus */
	bus = gst_element_get_bus(data.pipeline);

	do {
		msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

		/* parse message */
		if ( msg != NULL) {
			GError *err;
			gchar *debug_info;

			switch(GST_MESSAGE_TYPE(msg) ) {
				case GST_MESSAGE_ERROR:
					gst_message_parse_error(msg, &err, &debug_info);
					g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
					g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
					g_clear_error(&err);
					g_free(debug_info);
					terminate = TRUE;
					break;

				case GST_MESSAGE_EOS:
					g_print("End-Of-Stream reached.\n");
					terminate = TRUE;
					break;

				case GST_MESSAGE_STATE_CHANGED:

					/* we are only interested in state-changed messages from the pipeline */
					if ( GST_MESSAGE_SRC(msg) == GST_OBJECT(data.pipeline) ) {
						GstState old_state, new_state, pending_state;
						gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
						g_print("Pipeline state changed from %s to %s:\n", gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));
					}
					break;

				default:
					/* we should not reach here */
					g_printerr("Unexpected message received.\n");
					break;
			}

			gst_message_unref(msg);
		}
	} while (!terminate);


	/* free resources */
	gst_object_unref(bus);
	gst_element_set_state(data.pipeline, GST_STATE_NULL);
	gst_object_unref(data.pipeline);

	return 0;
}


/* This function will be called by the pad-added signal */
static void pad_added_handler(GstElement * src, GstPad *new_pad, CustomData *data)
{
	GstPadLinkReturn ret;
	GstCaps *new_pad_caps = NULL;
	GstStructure *new_pad_struct = NULL;
	const gchar *new_pad_type = NULL;

	g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));

	/* check the new pad's type */
	new_pad_caps = gst_pad_get_current_caps(new_pad);
	new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
	new_pad_type = gst_structure_get_name(new_pad_struct);

	if ( g_str_has_prefix(new_pad_type, "video/x-raw") ) {
		GstPad *video_q_pad = gst_element_get_static_pad(data->video_queue, "sink");

		/* if our converter is already linked, we have nothing to do here */
		if ( gst_pad_is_linked(video_q_pad)) {
			g_print("We are already linked. Ignoring. video\n");
			goto exit;
		}

		/* Attempt the link */
		ret = gst_pad_link(new_pad, video_q_pad);
		if ( GST_PAD_LINK_FAILED(ret) ) 
		{
			g_print("Type is '%s' but link failed. video\n", new_pad_type);
		}
		else
		{
			g_print("Link succeeded (type '%s'). video\n", new_pad_type);
		}

		/* Unreference the sink pad */
		gst_object_unref(video_q_pad);
	} else if ( g_str_has_prefix(new_pad_type, "audio/x-raw") ) {
		GstPad *audio_q_pad = gst_element_get_static_pad(data->audio_queue, "sink");

		if ( gst_pad_is_linked(audio_q_pad)) {
			g_print("We are already linked. Ignoring. audio\n");
			goto exit;
		}

		ret = gst_pad_link(new_pad, audio_q_pad);
		if ( GST_PAD_LINK_FAILED(ret) ) 
		{
			g_print("Type is '%s' but link failed. audio\n", new_pad_type);
		}
		else
		{
			g_print("Link succeeded (type '%s'). audio\n", new_pad_type);
		}

		gst_object_unref(audio_q_pad);
	} else {
		g_print("It has type '%s' which is not raw video. Ignoring.\n", new_pad_type);
		goto exit;
	}

exit:
	/* unreference the new pad's caps. if we got them */

	if ( new_pad_caps != NULL ) 
		gst_caps_unref(new_pad_caps);

}
