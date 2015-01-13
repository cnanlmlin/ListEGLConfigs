package android.egl.listeglconfigs;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.app.Activity;
import android.content.Context;
import android.view.Window;
import android.view.WindowManager;


class ListEGLConfigsView extends GLSurfaceView{

	public ListEGLConfigsView(Context context) {
		super(context);
		setRenderer(new Renderer());
	}
	
	protected static class Renderer implements GLSurfaceView.Renderer 
	{
		public void onDrawFrame(GL10 gl) 
		{       	
		}

		public void onSurfaceChanged(GL10 gl, int width, int height) 
        {
			ListEGLConfigs.init(width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) 
        {
        }
    }	
};

public class ListEGLConfigs extends Activity {

	ListEGLConfigsView mView;
	
	public static native void init(int width, int height);
	public static native void step();
	public static native void uninit();
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//setContentView(R.layout.activity_list_eglconfigs);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                             WindowManager.LayoutParams.FLAG_FULLSCREEN);
        
        mView = new ListEGLConfigsView(getApplication());
        setContentView(mView);
	}

    @Override protected void onPause() 
    {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() 
    {
        super.onResume();
        mView.onResume();
    }

    @Override protected void onDestroy()
    {
    	super.onDestroy();
    }

  static 
  {   
      System.loadLibrary("Native");        
  }

}
