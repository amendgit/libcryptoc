package com.ustc.crypto;

public class RawRC4 {
	
	static {
		System.loadLibrary("cryptoc");
	}
	
	private long instance;
	
	public native void RawNew(byte[] key);
	
	public native void RawXORStream(byte[] buf);
	
	public native void RawDestroy();
	
	public RawRC4(byte[] key) {
		System.loadLibrary("cryptopg");
		RawNew(key);
	}
}
