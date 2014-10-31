4399单机充值SDK Android客户端v1.0.0接入说明
======================

##修改记录
版本号  |   修改日期    |   修改者  |   修改内容
--------|---------------|-----------|-------------------------
v1.0.0  |   2014-10-31  |   张生    |   创建文档

# 文档说明
## 功能描述
4399单机版充值SDK（以下简称：SDK）主要用来向第三方游戏开发者提供便捷、安全一级可靠的多渠道单机游戏充值付费、版本升级检测等功能。本SDK适用于所有无需用户登录即可进行充值的Android游戏接入。本文主要描述SDK接口的使用方法，供合作伙伴的开发者接入使用。

## 阅读对象
本文档面向具有一定Android客户端开发能力，了解Android客户端的开发和管理人员。

## 开发包内容

 - 4399单机SDK（Android）接入说明：SDK接入文档，即本文档
 - m4399RechargeSDK：SDK资源文件工程内含SDK jar包和so库
 - m4399ConsoleOperateSDKDemo工程：Demo工程

# 集成流程

## 接入前期准备
1. 在[4399原创平台](http://www.4399api.com/)创建单机游戏
2. 填写游戏相关信息
3. 游戏创建成功后，可在原创平台获取接入所需的`GameKey`
4. GameKey为接入客户端SDK时使用，在初始化SDK时传入。

## SDK集成流程
假设现在你的工程目录名字叫project，下面将具体介绍如何将SDK接入project中。
### 关联资源工程
1. 将m4399ConsoleOperateSDK工程关联到project
* 将m4399ConsoleOperateSDK导入到eclipse中
* 右键点击4399ConsoleOperateSDK工程名→Properties→Android
* 勾选Is Library→OK
* 右键点击project工程名→Properties→Add
* 在弹出的对话框中点选资源工程m4399ConsoleOperateSDK→OK

### 配置AndroidManifest.xml文件
- 添加SDK所需的权限
``` xml
<uses-permission android:name="android.permission.CALL_PHONE"/>
<uses-permission android:name="android.permission.READ_PHONE_STATE" />
<uses-permission android:name="android.permission.MOUNT_UNMOUNT_FILESYSTEMS" 	/>
<uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
<uses-permission android:name="android.permission.INTERNET" />
<uses-permission android:name="android.permission.ACCESS_WIFI_STATE" />
<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
<uses-permission android:name="android.permission.SEND_SMS" />
<uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" />
<uses-permission android:name="android.permission.VIBRATE"></uses-permission>
```

- 注册SDK相关Activity&Service，注意必须放入`<application>`元素区块内
```xml
        <!-- For 4399 Recharge SDK -->
        <activity
            android:name="cn.m4399.recharge.ui.activity.RechargeActivity"
            android:launchMode="singleTask"
            android:configChanges="orientation|screenSize|keyboardHidden"
            android:theme="@style/m4399ActivityTheme" />
            
	<!--------以下为第三方支付SDK Activity&Service配置------------>
        <activity android:name="com.alipay.sdk.app.H5PayActivity" 
            android:screenOrientation="landscape"/>
```
* 注：第三方支付SDK的Activity需在AndroidManifest.xml中强制配置横竖屏，请游戏方根据游戏的横竖屏要求手工配置`landscape`|`portrait`


### 代码混淆配置
如果游戏有需要进行代码混淆，请不要混淆联编的jar包下的类，可以在`proguard.cfg`文件里追加以下配置排除SDK jar包中得类
```
-dontwarn android.support.v4.**
-keep class android.support.v4.** { *; }
-keep public class * extends android.support.v4.**

-dontwarn com.unipay.**
-keep class cn.m4399.operate.** {*;}
-keep class cn.m4399.recharge.** {*;}
-keepclassmembers class cn.m4399.recharge.R$* {*;}
```

# 接入流程
## 初始化
初始化推荐在游戏初始化过程中进行，析构函数则在游戏退出前执行。
```java
mOpeCenter = ConsleOperateCenter.getInstance();
new OperateCenterConfig.Builder(this)
	.setDebugEnabled(true)  //发布游戏时，要设为false
	.setOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT) //界面方向
	.setSupportExcess(true) //设置是否支持超出金额充值
	.setGameKey("70000") 	//换成实际游戏的gamekey
	.setGameName("测试游戏")	//换成实际游戏的名字，原则上与游戏名字匹配
	.build();
		
OnConsoleRechargeFinishedListener onRechargeFinishedListener = 
	new OnConsoleRechargeFinishedListener() {
		//充值结束，游戏在此根据不同情况做不同处理
		@Override
		public void onRechargeFinished(boolean success, int resultCode, 
			String msg) {
			Log.d(TAG, "Pay: [" + success + ", " + resultCode + ", " + msg + "]");
			showInToast(resultCode + ": " + msg);
		}
        
        //游戏在此发放物品，并返回发放的结果——true，发放成功；false， 发放失败
		@Override
		public boolean onDistributeGoodsFinished(RechargeOrder o) {
			Log.d(TAG, "单机充值发放物品, [" + o + "]");
			showInToast("发放物品, " + o)；
			return true;
		}
	};
mOpeCenter.init(MainActivity.this, onRechargeFinishedListener);
```
`是否支持处理超出部分金额`也可单独设置  
```java
mOpeCenter.setSupportExcess(support);
```
`能否支持处理超出部分金额`指在使用SDK充值时，由于用户选择的充值渠道不同，可能造成实际充值金额超出游戏下单时传入的金额。如果游戏能够正确处理超出部分的金额，则本接口传入true。如果无法支持处理超出部分的金额，则传入false，SDK将会根据传入金额自动隐藏无法满足充值金额的渠道（例：开发者设置SupportExcess为false，充值时传入7元，此时4399一卡通中无7元面额的充值卡，此时4399一卡通的充值渠道将自动隐藏）。*SupportExcess*默认为false。

*注：代码中`MainActivity`为当前Activity.下文的`mOpeCenter`指`ConsoleOperateCenter`实例，通过`getInstance()`静态方法获得。*   

## 检查更新
SDK将检查后台是否有新版本游戏上线，如果有，则显示更新内容，并提示用户升级。该升级为`增量升级`，后台在提交新版游戏时自动制作差分包，更新时用户只需下载APK文件中新旧版本有差异的部分。相关更新内容和版本提交事宜，请联系4399相关运营对接人员。  
4399SDK的增量升级分为  
- 全自动增量更新（无需操作，默认初始化完成）
- 自定义界面增量更新 （当游戏选择使用该策略时，上线前需与4399运营人员沟通）
自定义界面增量更新接入方法详见：[4399运营SDK增量升级说明](DeltaUpdate.md)  



## 充值
当用户需要充值时，可调用本接口启动充值中心界面。
```java
mOpeCenter.recharge(MainActivity.this, je, productName);
```
* `je`充值金额：4399充值中心仅支持整数金额充值，最小充值金额`1`元，最大不超过`50000`元。
* `mark`订单号：最大长度32位，由包含字母或数字组成的唯一字符串，该字段不可为空。
* `productName`商品名称：最长不超过8个字符。 如果传入商品名，充值中心将直接显示改商品名称，如果充值金额大于下单时传入的`je`时，将显示商品名+XXX游戏币，相关游戏币的兑换比例在接入时提供给运营人员配置。如果未传入商品名，则直接显示XXX游戏币。


## 获取状态信息
工具接口，用于将回调函数中的`resultCode`解析为中文的说明(充值接口recharge的resultCode对应的中文是回调中的msg)。
```java
String resultMessage = OperateCenter.getResultMsg(resultCode);
```

## 析构
游戏退出时调用本接口，释放SDK资源以及保存相关数据。
```java
mOpeCenter.destroy();
```

##显示SDK版本
```java
mOpeCenter.getVersion(); //查看SDK版本号
```
