package com.ustc.crypto;

public class RawAES {
	static {
		System.loadLibrary("cryptoc");
	}
	
	private long instance;
	
	public native boolean RawNew();
	
	public native boolean RawSetupEncKey(byte[] key);
	
	public native byte[] RawEncBuf(byte buffer[]);
	
	public native boolean RawSetupDecKey(byte[] key);
	
	public native byte[] RawDecBuf(byte buffer[]);
	
	public native void RawDestroy();
	
	public RawAES() {
		this.instance = 0;
		this.RawNew();
	}
	
	protected void finalize() {
		if (this.instance != 0) {
			this.RawDestroy();
		}
	}
}
