package cn.m4399.game;

import java.io.File;
import java.util.ArrayList;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ScrollView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import cn.m4399.operate.OperateCenterConfig;
import cn.m4399.operate.SingleOperateCenter;
import cn.m4399.operate.SingleOperateCenter.SingleRechargeListener;
import cn.m4399.operate.model.ApkCheckResult;
import cn.m4399.operate.model.GameUpgradeInfo;
import cn.m4399.operate.model.callback.Callbacks.OnCheckFinishedListener;
import cn.m4399.operate.model.callback.Callbacks.OnUpdateListener;
import cn.m4399.recharge.RechargeOrder;

public class MainActivity extends Activity {
	public static final String TAG = "4399SDK-GameActivity";
	public static final String TOAST_PREFIX = "【DEMO】";
	
	// SDK界面支持的四种方向配置
	public static final Integer[] mOrientations = new Integer[] { 
		ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE, // 0，横屏
		ActivityInfo.SCREEN_ORIENTATION_PORTRAIT, // 1，竖屏
		ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE, // 6，横屏180度旋转
		ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT // 7，竖屏180度旋转
	};
	
	public static final String[] ORIENTATION = new String[] {
		"横屏",
		"竖屏",
		"横屏180度",
		"竖屏180度"
	};

	String toastPre = "【DEMO】";

	SingleOperateCenter mOpeCenter;
	OperateCenterConfig mOpeConfig;
	SharedPreferences mSp;
	
	private ArrayList<String> mSKUList; //临时存储购买成功的物品，当activity在活动时显示出来

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		mSp = getSharedPreferences("sdk_sp", MODE_PRIVATE);
		// 设置DemoActivity的方向
		setDemoOrientation();
		setContentView(R.layout.main);
		
		initOrientationSpinner();
		//初始化SDK
		initSDK();

		//显示SDK版本信息
		showSDKVersion();
		
		//初始化物品列表
		initSKUList();
	}

	private void initSKUList() {
		mSKUList = new ArrayList<String>();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();

		mOpeCenter.destroy();
		mOpeCenter = null;
	}
	
	private void showSDKVersion() {
		TextView versionText = (TextView) findViewById(R.id.text_version);
		if (versionText != null) {
			versionText.setText("单机充值SDK v" + SingleOperateCenter.getVersion());
		}
	}
	
	private void initSDK() {
		mOpeCenter = SingleOperateCenter.getInstance();
		new OperateCenterConfig.Builder(this)
			.setDebugEnabled(true)  //发布游戏时，要设为false
			.setOrientation(getOrientation()) //设置SDK界面方向，应与游戏设置一直
			.setSupportExcess(true) //设置是否支持超出金额充值
			.setGameKey("70001") 	//换成实际游戏的gamekey
			.setGameName("测试游戏")	//换成实际游戏的名字，原则上与游戏名字匹配
			.build();
		
		SingleRechargeListener singleRechargeListener = new SingleRechargeListener() {
			
			/*
			 * 充值过程结束时SDK回调此方法
			 * 
			 * @param resultCode
			 * 	表示充值结果的状态码
			 * @param msg
			 *  表示充值结果的友好的文本说明
			 *  
			 *  充值过程结束并不代表订单生命周期全部完成，SDK还需要查询订单状态，游戏
			 *  要根据订单状态决定是否发放物品等
			 */
			@Override
			public void onRechargeFinished(int resultCode, String msg) {
				Log.d(TAG, "Pay: [" + resultCode + ", " + msg + "]");
				showInToast(resultCode + ": " + msg);
			}
			
			/*
			 * 充值过程成功完成后，SDK会查询订单状态，根据订单状态状态正常则通知游戏发放物品
			 * 
			 * @param shouldDeliver
			 *  是否要发放物品
			 * @param o
			 *  封装了最后提交的订单信息的对象，主要包含以下成员，各成员都有getter方法
			 *  payChannel：   充值渠道
			 *  orderId：      	充值订单号
			 *  je：			充值金额
			 *  goods：        	购买的物品
			 * @return 
			 *  物品发放过程是否成功
			 */

			@Override
			public boolean notifyDeliverGoods(boolean shouldDeliver, RechargeOrder o) {
				if (shouldDeliver) {
					Log.d(TAG, "单机充值发放物品, [" + o + "]");
					showInToast("发放物品, " + o);
					mSKUList.add("您花费 " + o.getJe() + "元， 购买了 " + o.getGoods());

					return true;
				} else {
					Log.d(TAG, "单机充值查询定单出错，建议不要发放物品");
					return false;
				}
			}
		};
		mOpeCenter.init(MainActivity.this, singleRechargeListener);
	}
	
	/*
	 * 测试SDK界面的方向设置
	 * 
	 * 为了在Demo中测试横竖屏，提供Spinner控件设置方向配置，并保持着SharedPreference中
	 * 实际接入直接设置OperateCenterConfig
	 * .Builder(this)。setOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE)
	 * 
	 * 注意：横竖屏设置都需要重启Demo才能生效; 测试旋转屏幕时要开启“自动旋转屏幕”
	 */
	private void setDemoOrientation() {
		int ori = getOrientation();
		setRequestedOrientation(ori);
	}

	public void initOrientationSpinner() {
		Spinner spinner = (Spinner) findViewById(R.id.m4399_demo_ori_spinner);

		ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, ORIENTATION);
		adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		spinner.setAdapter(adapter);

		spinner.setOnItemSelectedListener(new OnItemSelectedListener() {
			@Override
			public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
				Log.d(TAG, "Set orientation as " + mOrientations[pos]);
				saveOrientation(mOrientations[pos]);
			}

			@Override
			public void onNothingSelected(AdapterView<?> arg0) {
			}
		});

		// 这里设置默认方向为横屏，SDK内部的默认方向也是
		spinner.setSelection(getOriIndex());
	}

	private int getOriIndex() {
		int index = 0;
		int orientation = getOrientation();
		for (int i = 0; i < mOrientations.length; i++) {
			if (orientation == mOrientations[i])
				index = i;
		}
		return index;
	}

	private int getOrientation() {
		int orientation = mSp.getInt("orientation", ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		return orientation;
	}

	private void saveOrientation(int orientation) {
		for (int i = 0; i < mOrientations.length; i++) {
			if (mOrientations[i] == orientation)
				mSp.edit().putInt("orientation", mOrientations[i]).commit();
		}
	}
	/******************* 结束横竖屏设置和测试代码 **********************/

	/**
	 * 测试自定义增量更新
	 * 
	 * 自定义增量更新允许开发者定制升级界面
	 * 
	 */
	public void onUpdateButtonClicked(View view) {
		Log.d(TAG, "Update Button Clicked...");

		mOpeCenter.checkUpdateApk(MainActivity.this, new OnCheckFinishedListener() {
			@Override
			public void onCheckResponse(ApkCheckResult checkResultInfo, String newApkFilePath) {
				Log.d(TAG, checkResultInfo + ": " + newApkFilePath);
				showCheckResult(checkResultInfo);
			}
		});
	}

	//显示更新结果
	private void showCheckResult(ApkCheckResult result) {
		int code = result.getCode();
		final GameUpgradeInfo updateInfo = result.getNewApkInfo();
		StringBuilder msgBuilder = new StringBuilder();

		if (code == 0x00) {
			msgBuilder.append("\n检查结果：" + "已经是最新版本");
		} else if (code == 0x01) {
			if (updateInfo != null)
				msgBuilder.append("\n检查结果：")
				.append("\n版本号: ")
				.append(updateInfo.getVersion())
				.append("\n时间: ")
				.append(updateInfo.getDate())
				.append("\n是否强制更新：")
				.append(updateInfo.isCompel())
				.append("\n补丁/游戏大小：")
				.append(updateInfo.getPatchSize() + "/" + updateInfo.getGameSizeByte())
				.append("\n更新内容：")
				.append(updateInfo.getUpdateMsg());
			else
				msgBuilder.append("\ncode: ").append(code).append("\n检查结果: ").append("(更新结果)");
		} else {
			msgBuilder.append("\n检查结果：" + "检查更新失败");
		}

		String action = updateInfo.isHaveDownApk() ? "安装更新包" : "开始更新";
		Builder builder = new Builder(MainActivity.this);
		builder.setTitle("自定义更新").setMessage(msgBuilder).setPositiveButton(action, new DialogInterface.OnClickListener() {

			@Override
			public void onClick(DialogInterface dialog, int which) {
				doUpdate(updateInfo);
			}

		}).setNegativeButton("取消更新", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
			}

		}).create().show();
	}

	private void doUpdate(final GameUpgradeInfo info) {
		final ProgressDialog dialog = new ProgressDialog(MainActivity.this);
		dialog.setMessage("准备下载。。。");
		dialog.show();

		if (info.isHaveDownApk())
			dialog.dismiss();
		mOpeCenter.doUpdate(MainActivity.this, info, new OnUpdateListener() {

			@Override
			public boolean onUpdateSuccess(File newApkFile) {
				Log.d(TAG, "onUpdateSuccess");
				dialog.setMessage("下载成功...");
				dialog.dismiss();

				return false;
			}

			@Override
			public void onUpdateFail(int resultCode) {
				Log.d(TAG, "onUpdateFail");
				dialog.setMessage("下载失败...");
				dialog.dismiss();
			}

			@Override
			public void onUpdateStart() {
				Log.d(TAG, "onUpdateStart");
				dialog.setMessage("开始更新...");
			}

			@Override
			public void onUpdateProgress(long progress, long max) {
				long percentage = progress * 100 / max;
				dialog.setMessage("正在更新, 更新进度：" + percentage + "%");
			}
		});
	}
	/**************** 结束自定义更新测试 **********************/

	public void onRechargeButtonClicked(View view) {
		Log.d(TAG, "Pay Button Clicked...");

		LayoutInflater inflator = (LayoutInflater) getSystemService(LAYOUT_INFLATER_SERVICE);
		final ScrollView sv = (ScrollView) inflator.inflate(R.layout.params_scrollview, null);
		AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);

		builder.setTitle(R.string.start_pay).setView(sv).setNegativeButton("Cancel", new DialogInterface.OnClickListener() {

			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
			}
		}).setPositiveButton("OK", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				TextView jeTV = (TextView) sv.findViewById(R.id.je);
				String je = jeTV.getText().toString();

				TextView subjectTV = (TextView) sv.findViewById(R.id.subject);
				String productName = subjectTV.getText().toString();

				//是否支持超出金额，这里是一个独立的接口，只要在充值之前调用都有作用
				CheckBox chCB = (CheckBox) sv.findViewById(R.id.changable);
				boolean supportExcess = chCB.isChecked();
				mOpeCenter.setSupportExcess(supportExcess);

				//是否传入商品名，这会影响商品在充值界面的显示
				CheckBox hsCB = (CheckBox) (CheckBox) sv.findViewById(R.id.have_subject);
				boolean hasSubject = hsCB.isChecked();
				if (!hasSubject) {
					productName = null;
				}

				Log.d(TAG, "[" + je  + ", " + productName + ", " + supportExcess + "]");

				mOpeCenter.recharge(MainActivity.this, je, productName);
				dialog.dismiss();
			}
		}).create().show();
	}
	
	public void onPurchaseButtonClicked(View view) {
		LinearLayout container = (LinearLayout)LayoutInflater.from(MainActivity.this).inflate(R.layout.product_list, null);
		ListView skuListView = (ListView) container.findViewById(R.id.product_list);
		skuListView.setAdapter(new ArrayAdapter<String>(MainActivity.this, android.R.layout.simple_expandable_list_item_1, mSKUList));
		
		AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this)
			.setTitle("已成功购买的物品")
			.setView(container)
			.setPositiveButton("确定", new OnClickListener() {

				@Override
				public void onClick(DialogInterface dialog, int which) {
					dialog.dismiss();
				}
			});
		
		builder.create().show();
	}
	
	private void showInToast(String msg) {
		Toast.makeText(MainActivity.this, TOAST_PREFIX + msg, Toast.LENGTH_SHORT).show();
	}
}
