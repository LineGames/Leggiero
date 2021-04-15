package com.leggiero.Engine.Service;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.view.View;
import android.view.MenuItem;
import android.util.Log;
import android.widget.Button;
import android.net.http.SslError;
import android.webkit.SslErrorHandler;
import android.graphics.Color;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.view.KeyEvent;
import java.util.Map;
import java.util.HashMap;
import android.os.Build;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;

import android.content.Context;
import android.content.res.Resources;

import com.leggiero.Engine.Service.LeggieroWebView;


public class URLActivity extends Activity
{
	static WebView m_webView = null;
	static String m_url = "";
	static String m_backButtonName = "";
	static Map<String, String> m_customHeader;
	static String m_lgagent = "";

    @SuppressLint("SetJavaScriptEnabled")
    @Override
    protected void onCreate(Bundle savedInstanceState)
	{
        LeggieroWebView.s_isURLPageOpened = true;

		Context context = this.getApplicationContext();
		Resources r = context.getResources();
		int layoutwebview = r.getIdentifier("webview", "layout", context.getPackageName());
		int idwebview = r.getIdentifier("webview", "id", context.getPackageName());
		int btn_close = r.getIdentifier("btn_close", "id", context.getPackageName());
        super.onCreate(savedInstanceState);
        setContentView(layoutwebview);

		getActionBar().setTitle(m_backButtonName);
		getActionBar().setIcon(android.R.color.transparent);
		getActionBar().setDisplayHomeAsUpEnabled(true);
		
        URLActivity.m_webView = (WebView)findViewById(idwebview);
        URLActivity.m_webView.setInitialScale(1);
        URLActivity.m_webView.getSettings().setJavaScriptEnabled(true);
        URLActivity.m_webView.getSettings().setLoadWithOverviewMode(true);
        URLActivity.m_webView.getSettings().setUseWideViewPort(true);
        URLActivity.m_webView.setScrollBarStyle(WebView.SCROLLBARS_OUTSIDE_OVERLAY);
        URLActivity.m_webView.setScrollbarFadingEnabled(false);

		String nativeAgent = "AddNativeAgentHere";
		Log.i("Leggiero", "http agent");
		Log.i("Leggiero", nativeAgent);

		m_customHeader = new HashMap<String,String>();
		m_customHeader.put("lgagent",nativeAgent);
		m_lgagent = nativeAgent;

		String userAgent = URLActivity.m_webView.getSettings().getUserAgentString();
    	URLActivity.m_webView.getSettings().setUserAgentString(userAgent + " [LGAGENT]" + nativeAgent + "[LGAGENT]");
	
		if (Build.VERSION.SDK_INT >= 21) {
			URLActivity.m_webView.setWebViewClient(new WebViewClient() {
				
           @Override
            public void onReceivedSslError(WebView view, final SslErrorHandler handler, SslError error) {			
                AlertDialog.Builder builder = new AlertDialog.Builder(URLActivity.this);
                builder.setMessage("page with invalid ssl certificate. continue?");
                builder.setPositiveButton("yes", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        handler.proceed();
                    }
                });
                builder.setNegativeButton("no", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        handler.cancel();
                    }
                });
                builder.setOnKeyListener(new DialogInterface.OnKeyListener() {
                    @Override
                    public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                        if (event.getAction() == KeyEvent.ACTION_UP && keyCode == KeyEvent.KEYCODE_BACK) {
                            handler.cancel();
                            dialog.dismiss();
                            return true;
                        }
                        return false;
                    }
                });
                AlertDialog dialog = builder.create();
                dialog.show();
			}

			 @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                view.loadUrl(url, URLActivity.m_customHeader);
                return true;

            } // end shouldOverrideUrlLoading

			@Override
			public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
				request.getRequestHeaders().put("lgagent", m_lgagent);
				return super.shouldInterceptRequest(view, request);
			} // shouldInterceptRequest
			
			} // end new WebViewClient
		);
    
		} // end Build.VERSION.SDK_INT >= 21
		else
		{
		URLActivity.m_webView.setWebViewClient(new WebViewClient() {
				
           @Override
            public void onReceivedSslError(WebView view, final SslErrorHandler handler, SslError error) {			
                AlertDialog.Builder builder = new AlertDialog.Builder(URLActivity.this);
                builder.setMessage("page with invalid ssl certificate. continue?");
                builder.setPositiveButton("yes", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        handler.proceed();
                    }
                });
                builder.setNegativeButton("no", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        handler.cancel();
                    }
                });
                builder.setOnKeyListener(new DialogInterface.OnKeyListener() {
                    @Override
                    public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                        if (event.getAction() == KeyEvent.ACTION_UP && keyCode == KeyEvent.KEYCODE_BACK) {
                            handler.cancel();
                            dialog.dismiss();
                            return true;
                        }
                        return false;
                    }
                });
                AlertDialog dialog = builder.create();
                dialog.show();
            
			}

			 @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                view.loadUrl(url, URLActivity.m_customHeader);
                return true;

            } // end shouldOverrideUrlLoading
			
			} // end new WebViewClient
		);
		} // end else

        URLActivity.m_webView.loadUrl(URLActivity.m_url,URLActivity.m_customHeader);
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        LeggieroWebView.s_isURLPageOpened = false;
    }

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case android.R.id.home:
			finish();
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
}
