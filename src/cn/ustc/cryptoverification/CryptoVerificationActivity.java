package cn.ustc.cryptoverification;

import java.security.InvalidKeyException;
import java.security.SecureRandom;
import java.util.Random;

import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.widget.TextView;

import com.mobilepg.crypto.RawAES;
import com.mobilepg.crypto.RawRC4;

public class CryptoVerificationActivity extends Activity {
	
	private TextView textView;
	
	byte[] encrypt(String passwd, byte[] plainTextBytes) {
		try {
			// Get key generator.
			KeyGenerator keyGenerator = KeyGenerator.getInstance("AES");
			// Get random generator and set the seed.
			SecureRandom secureRandom = SecureRandom.getInstance("SHA1PRNG");
			secureRandom.setSeed(passwd.getBytes());
			// Generate the raw key.
			keyGenerator.init(128, secureRandom);
			SecretKey secretKey = keyGenerator.generateKey();
			// 
			SecretKeySpec secretKeySpec = new SecretKeySpec(secretKey.getEncoded(), "AES");
			// 
			Cipher cipher = Cipher.getInstance("AES");
			cipher.init(Cipher.ENCRYPT_MODE, secretKeySpec);
			byte[] cipherTextBytes = cipher.doFinal(plainTextBytes);
			// Done
			return cipherTextBytes;
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}
	
	byte[] decrypt(String passwd, byte[] cipherTextBytes) {
		try {
			KeyGenerator keyGenerator = KeyGenerator.getInstance("AES");
			SecureRandom secureRandom = SecureRandom.getInstance("SHA1PRNG");
			secureRandom.setSeed(passwd.getBytes());
			keyGenerator.init(128, secureRandom);
			SecretKey secretKey = keyGenerator.generateKey();
			SecretKeySpec secretKeySpec = new SecretKeySpec(secretKey.getEncoded(), "AES");
			Cipher cipher = Cipher.getInstance("AES");
			cipher.init(Cipher.DECRYPT_MODE, secretKeySpec);
			byte[] plainTextBytes = cipher.doFinal(cipherTextBytes);
			return plainTextBytes;
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}
	
	void aesUnitTest(String key, byte[] p, int n) {
		byte[] c = encrypt(key, p);
		byte[] p2 = decrypt(key, c);
		for (int i = 0; i < p.length; i++) {
			if (p[i] != p2[i]) {
				output("test data " + n + " failed\n");
				return ;
			}
		}
		output("test data " + n + " pass\n");
	}
	
	void aesTestCase() {
		aesUnitTest("test", "12345678".getBytes(), 1);
		aesUnitTest("totem", "fuck you!".getBytes(), 2);
	}
	
	void testAes512kb() {
		byte[] data = new byte[512 * 1024];
		Random random = new Random();
		random.nextBytes(data);
		
		output("Test AES 512kb start:\n");
		long startTime = System.currentTimeMillis();
		
		byte[] unused = encrypt("totem", data);		
		
		long endTime = System.currentTimeMillis();
		
		output("Done: " + (endTime - startTime) + "\n");
	}
	
	byte[] encryptAesCbcPkcs5(String passwd, byte[] plainTextBytes) {
		try {
			// Get key generator.
			KeyGenerator keyGenerator = KeyGenerator.getInstance("AES");
			// Get random generator and set the passwd as seed.
			SecureRandom secureRandom = SecureRandom.getInstance("SHA1PRNG");
			secureRandom.setSeed(passwd.getBytes());
			// Generate the raw key.
			keyGenerator.init(128, secureRandom);
			SecretKey secretKey = keyGenerator.generateKey();
			//
			SecretKeySpec secretKeySpec = new SecretKeySpec(secretKey.getEncoded(), "AES");
			// 
			Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
			IvParameterSpec iv = new IvParameterSpec("0102030405060708".getBytes());
			cipher.init(Cipher.ENCRYPT_MODE, secretKeySpec, iv);
			byte[] cipherTextBytes = cipher.doFinal(plainTextBytes);
			return cipherTextBytes;
		} catch (Exception e) {
			e.printStackTrace();
		} 
		return null;
	}
	
	byte[] decryptAesCbcPkcs5(String passwd, byte[] cipherTextBytes) {
		try {
			// Get key generator.
			KeyGenerator keyGenerator = KeyGenerator.getInstance("AES");
			// Get random generator and set the passwd as seed.
			SecureRandom secureRandom = SecureRandom.getInstance("SHA1PRNG");
			secureRandom.setSeed(passwd.getBytes());
			// Generate the raw key.
			keyGenerator.init(128, secureRandom);
			SecretKey secretKey = keyGenerator.generateKey();
			//
			SecretKeySpec secretKeySpec = new SecretKeySpec(secretKey.getEncoded(), "AES");
			// 
			Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
			IvParameterSpec iv = new IvParameterSpec("0102030405060708".getBytes());
			cipher.init(Cipher.DECRYPT_MODE, secretKeySpec, iv);
			byte[] plainTextBytes = cipher.doFinal(cipherTextBytes);
			return plainTextBytes;
		} catch (Exception e) {
			Log.e("AES Unit Test",e.getLocalizedMessage());
			e.printStackTrace();
		} 
		return null;
	}
	
	void aesCbcPkcs5UnitTest(String key, byte[] p, int n) {
		byte[] c = encryptAesCbcPkcs5(key, p);
		byte[] p2 = decryptAesCbcPkcs5(key, c);
		
		for (int i = 0; i < p.length; i++) {
			if (p[i] != p2[i]) {
				output("AES unit test " + n + " fail.\n");
				return ;
			}
		}
		output("AES unit test " + n + " pass.\n");
	}
	
	void aesCbcPkcs5TestCase() {
		aesCbcPkcs5UnitTest("totem", "12345".getBytes(), 1);		
	}
	
	void testAesCbcPkcs5Padding512kb() {
		byte[] data = new byte[512 * 1024];
		Random random = new Random();
		random.nextBytes(data);
		
		output("Test AES CBC PACS5Padding 512kb start:\n");
		long startTime = System.currentTimeMillis();
		
		encryptAesCbcPkcs5("totemabc", data);
		
		long endTime = System.currentTimeMillis();
		
		output("Done: " + (endTime - startTime) + "\n");
	}
	
	void rc4Test(byte[] key, byte[] keystream, int n) {
		byte[] work = new byte[keystream.length];
		for (int i = 0; i < work.length; i++) {
			work[i] = 0;
		}
		
		RC4 rc4 = new RC4();
		try {
			rc4.init(key);
		} catch (InvalidKeyException e) {
			e.printStackTrace();
		}
		rc4.xorKeyStream(work, work);
		
		for (int i = 0; i < work.length; i++) {
			if (work[i] != keystream[i]) {
				output("test data " + n + " error.\n");
				break;
			}
		}
		rc4.reset();
	}
	
	void rc4TestCase() {
		byte[] key1 = {0x01, 0x23, 0x45, 0x67, (byte) 0x89, (byte) 0xab, (byte) 0xcd, (byte) 0xef};
		byte[] keystream1 = {0x74,  (byte) 0x94, (byte) 0xc2, (byte) 0xe7, 0x10, 0x4b, 0x08, 0x79};
		rc4Test(key1, keystream1, 1);
		
		byte[] key2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		byte[] keystream2 = {(byte) 0xde, 0x18, (byte) 0x89, 0x41, (byte) 0xa3, 0x37, 0x5d, 0x3a};
		rc4Test(key2, keystream2, 2);
		
		byte[] key3 = {(byte) 0xef, 0x01, 0x23, 0x45};
		byte[] keystream3 = {(byte) 0xd6, (byte) 0xa1, 0x41, (byte) 0xa7, (byte) 0xec, 0x3c, 0x38, (byte) 0xdf, (byte) 0xbd, 0x61};
		rc4Test(key3, keystream3, 3);
		
		byte[] key4 = {0x4b, 0x65, 0x79};
		byte[] keystream4 = {(byte) 0xeb, (byte) 0x9f, 0x77, (byte) 0x81, (byte) 0xb7, 0x34, (byte) 0xca, 0x72, (byte) 0xa7, 0x19};
		rc4Test(key4, keystream4, 4);
		
		byte[] key5 = {0x57, 0x69, 0x6b, 0x69};
		byte[] keystream5 = {0x60, 0x44, (byte) 0xdb, 0x6d, 0x41, (byte) 0xb7};
		rc4Test(key5, keystream5, 5);
		
		output("rc4 test done.\n");
	}
	
	void testRC4512kb() {
		byte[] data = new byte[512 * 1024];
		Random random = new Random();
		random.nextBytes(data);
		
		output("Test RC4 512kb start:\n");
		long startTime = System.currentTimeMillis();
		
		RC4 rc4 = new RC4();
		try {
			rc4.init("totem".getBytes());
		} catch(InvalidKeyException e) {
			e.printStackTrace();
		}
		rc4.xorKeyStream(data, data);
		
		long endTime = System.currentTimeMillis();
		
		output("Done: " + (endTime - startTime) + "\n");
	}
	
	public void natvieAESUnitTest(String key, byte[] p, int n) {
		RawAES a = new RawAES();
		a.RawNew();
		
		a.RawSetupEncKey(key.getBytes());
		
		byte[] c = a.RawEncBuf(p);
		a.RawSetupDecKey(key.getBytes());
		
		byte[] p1 = a.RawDecBuf(c);
		
		for (int i = 0; i < p.length; i++) {
			if (p[i] != p1[i]) {
				output("AES unit test " + n + " fail.\n");
				return ;
			}
		}
		output("NativeAES unit test " + n + " pass.\n");
	} 
	
	void testNativeAES512KB() {
		byte[] data = new byte[512*1024];
		Random random = new Random();
		random.nextBytes(data);
		
		output("Test NativeAES 512kb start:\n");
		long startTime = System.currentTimeMillis();
		
		RawAES na = new RawAES();
		na.RawSetupEncKey("1234567890123456".getBytes());
		byte[] c = na.RawEncBuf(data);
		//na.RawSetupDecKey("1234567890123456".getBytes());
		//na.RawDecBuf(c);
		
		long endTime = System.currentTimeMillis();
		
		output("Done: " + (endTime - startTime) + "\n");
	}
	
	public void nativeAESTestCase() {
		natvieAESUnitTest("1234567890123456", "hehe".getBytes(), 1);
		natvieAESUnitTest("1234567890123456", "abcdefghijklmnopqrstuvwxyz".getBytes(), 2);
	}
	
	void rawRc4Test(byte[] key, byte[] keystream, int n) {
		byte[] work = new byte[keystream.length];
		for (int i = 0; i < work.length; i++) {
			work[i] = 0;
		}
		
		RawRC4 rc4 = new RawRC4(key);
		
		rc4.RawXORStream(work);
		
		for (int i = 0; i < work.length; i++) {
			if (work[i] != keystream[i]) {
				output("test data " + n + " error.\n");
				return ;
			}
		}
		output("test case" + n + "pass.\n");
	}
	
	public void rawRC4TestCase() {
		byte[] key1 = {0x01, 0x23, 0x45, 0x67, (byte) 0x89, (byte) 0xab, (byte) 0xcd, (byte) 0xef};
		byte[] keystream1 = {0x74,  (byte) 0x94, (byte) 0xc2, (byte) 0xe7, 0x10, 0x4b, 0x08, 0x79};
		rc4Test(key1, keystream1, 1);
		
		byte[] key2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		byte[] keystream2 = {(byte) 0xde, 0x18, (byte) 0x89, 0x41, (byte) 0xa3, 0x37, 0x5d, 0x3a};
		rc4Test(key2, keystream2, 2);
		
		byte[] key3 = {(byte) 0xef, 0x01, 0x23, 0x45};
		byte[] keystream3 = {(byte) 0xd6, (byte) 0xa1, 0x41, (byte) 0xa7, (byte) 0xec, 0x3c, 0x38, (byte) 0xdf, (byte) 0xbd, 0x61};
		rc4Test(key3, keystream3, 3);
		
		byte[] key4 = {0x4b, 0x65, 0x79};
		byte[] keystream4 = {(byte) 0xeb, (byte) 0x9f, 0x77, (byte) 0x81, (byte) 0xb7, 0x34, (byte) 0xca, 0x72, (byte) 0xa7, 0x19};
		rc4Test(key4, keystream4, 4);
		
		byte[] key5 = {0x57, 0x69, 0x6b, 0x69};
		byte[] keystream5 = {0x60, 0x44, (byte) 0xdb, 0x6d, 0x41, (byte) 0xb7};
		
		rawRc4Test(key1, keystream1, 1);
		rawRc4Test(key2, keystream2, 2);
		rawRc4Test(key3, keystream3, 3);
		rawRc4Test(key4, keystream4, 4);
		rawRc4Test(key5, keystream5, 5);
	}
	
	void output(String string) {
		textView.append(string);
	}
	
	private void testRawRC4() {
		byte[] data = new byte[512 * 1024];
		Random random = new Random();
		random.nextBytes(data);
		
		output("Test RawRC4 512kb start:\n");
		long startTime = System.currentTimeMillis();
		
		RawRC4 rc4 = new RawRC4("totem".getBytes());
		rc4.RawXORStream(data);
		
		long endTime = System.currentTimeMillis();
		
		output("Done: " + (endTime - startTime) + "\n");
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		textView = new TextView(this);
		textView.setText("Begin to test\n");
		setContentView(textView);
		
		// aesTestCase();
		testAes512kb();
		
		// rc4TestCase();
		testRC4512kb();
		
		// aesCbcPkcs5TestCase();
		testAesCbcPkcs5Padding512kb();
		
		// nativeAESTestCase();
		testNativeAES512KB();
		
		// rawRC4TestCase();
		testRawRC4();
	}



	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_crypto_verification, menu);
		return true;
	}
}
