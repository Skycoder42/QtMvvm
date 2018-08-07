package de.skycoder42.qtmvvm.core;

import java.util.Set;

import android.content.Context;
import android.content.SharedPreferences;

import android.preference.PreferenceManager;

class AndroidSettingsAccessor {
	private long address = 0;
	private SharedPreferences preferences = null;
	private SharedPreferences.Editor editor = null;
	private Listener listener = new Listener();

	private class Listener implements SharedPreferences.OnSharedPreferenceChangeListener {
		@Override
		public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
			if(address != 0 && preferences.equals(sharedPreferences))
				callback(address, key);
		}
	}

	private static native void callback(long address, Object key);

	public AndroidSettingsAccessor(Context context, long address) {
		this.address = address;
		preferences = PreferenceManager.getDefaultSharedPreferences(context);
		preferences.registerOnSharedPreferenceChangeListener(listener);
	}

	public AndroidSettingsAccessor(Context context, String name, int mode, long address) {
		this.address = address;
		preferences = context.getSharedPreferences(name, mode);
		preferences.registerOnSharedPreferenceChangeListener(listener);
	}

	public void unref() {
		address = 0;
		preferences.unregisterOnSharedPreferenceChangeListener(listener);
		listener = null;
		sync();
	}

	public boolean contains(String key) {
		return preferences.contains(key);
	}

	public Object load(String key) {
		return preferences.getAll().get(key);
	}

	public boolean save(String key, boolean value) {
		boolean res = beginEdit();
		editor.putBoolean(key, value);
		return res;
	}

	public boolean save(String key, int value) {
		boolean res = beginEdit();
		editor.putInt(key, value);
		return res;
	}

	public boolean save(String key, long value) {
		boolean res = beginEdit();
		editor.putLong(key, value);
		return res;
	}

	public boolean save(String key, float value) {
		boolean res = beginEdit();
		editor.putFloat(key, value);
		return res;
	}

	public boolean save(String key, String value) {
		boolean res = beginEdit();
		editor.putString(key, value);
		return res;
	}

	public boolean save(String key, Set<String> value) {
		boolean res = beginEdit();
		editor.putStringSet(key, value);
		return res;
	}

	public boolean remove(String key) {
		boolean res = beginEdit();
		editor.remove(key);
		return res;
	}

	public void sync() {
		if(editor != null) {
			editor.apply();
			editor = null;
		}
	}

	private boolean beginEdit() {
		if(editor != null)
			return false;
		else {
			editor = preferences.edit();
			return true;
		}
	}
}
