﻿4399单机充值SDK Android客户端v1.0.0接入说明
======================

## 修改记录  

版本号  |   修改日期    |   修改者  |   修改内容  
--------|---------------|-----------|-------------------------  
v1.0.0  |   2014-10-31  |   张生    |   创建文档  
v1.1.1  |   2015-04-25  |   张生    |   增加历史订单查询功能，调整充值完成接口  
v1.1.2  |   2015-10-13  |   张生    |   增加带透传字段的充值接口的说明  
v1.4.0  |   2019-09-10  |   涂仕聪    |   为独立开发者提供礼包接口  
v1.6.0  |   2020-02-19  |   涂仕聪    |   新增激活码验证功能  
v1.6.2  |   2020-03-11  |   涂仕聪    |   激活码功能优化、修复bug
v1.7.0  |   2020-03-17  |   涂仕聪    |   替换https协议、修改礼包兑换文本提示
v2.0.0  |   2020-06-18  |   涂仕聪    |   重构SDK

# 文档说明
## 功能描述
4399 Android 单机SDK（以下简称：SDK）向游戏开发者提供便捷、可靠的多渠道充值付费、版本升级检测等功能。  
本文主要描述SDK接口的使用方法，供游戏开发者接入使用。

## 阅读对象
本文档面向具有一定Android客户端开发能力，了解 Android 客户端的开发和管理人员。

## 开发包内容

 - 4399单机SDK（Android）接入说明：SDK接入文档，即本文档
 - m4399Single：SDK资源文件工程内含SDK jar包和so库
 - m4399SingleDemo工程：Demo工程

# 集成流程
<img src="Images/work_flow.png" alt="集成流程图" />

## 接入前期准备
1. 在[4399原创平台](http://www.4399api.com/)创建单机游戏  
2. 填写游戏相关信息  
3. 游戏创建成功后，可在原创平台获取接入所需的游戏id，即`GameKey`  
4. `GameKey`为接入客户端SDK时使用，在初始化SDK时传入。  

## SDK集成流程
假设现在你的工程目录名字叫project，下面将具体介绍如何将SDK接入project中。
### 关联资源工程
1. 将m4399Single工程关联到project
* 将m4399Single导入到eclipse中
* 右键点击工程名→Properties→Android
* 勾选Is Library→OK
* 右键点击project工程名→Properties→Add
* 在弹出的对话框中点选资源工程m4399Single→OK


### 配置AndroidManifest.xml文件
- 添加SDK所需的权限

``` xml
    <uses-permission android:name="android.permission.READ_PHONE_STATE" />
    <uses-permission android:name="android.permission.INTERNET" />
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />
    <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE" />
    <uses-permission android:name="android.permission.ACCESS_WIFI_STATE" />
    <uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
    <uses-permission android:name="android.permission.VIBRATE" />
    <uses-permission android:name="android.permission.REQUEST_INSTALL_PACKAGES" />
```

- 注册SDK相关Activity&Service，注意必须放入`<application>`元素区块内
```xml
<application
	...
        android:allowBackup="false"
        android:networkSecurityConfig="@xml/m4399single_network_security">
	
	<!-- For 7.0 FileProvider -->
	<provider
		android:name="android.support.v4.content.FileProvider"
		android:authorities="${applicationId}.single.FileProvider"
		android:exported="false"
		android:grantUriPermissions="true">
		<meta-data
		   android:name="android.support.FILE_PROVIDER_PATHS"
		   android:resource="@xml/m4399single_file_paths" />
	</provider>

        <receiver android:name="cn.m4399.single.gamebox.Installation$DownloadReceiver">
            <intent-filter>
                <action android:name="android.intent.action.DOWNLOAD_COMPLETE" />
                <action android:name="android.intent.action.DOWNLOAD_NOTIFICATION_CLICKED" />
            </intent-filter>
        </receiver>

	<!-- 4399 single activities，可选替换 demo_screen_orientation-->
        <activity
            android:name="cn.m4399.single.component.HostActivity"
            android:configChanges="orientation|screenSize|keyboardHidden"
            android:screenOrientation="${demo_screen_orientation}"
            android:theme="@style/m4399.Theme.Activity.Host"
            tools:replace="android:screenOrientation" />

        <!-- 第三方 Activity 方向应设置与游戏一致，替换 demo_screen_orientation -->
        <activity
            android:name="com.alipay.sdk.app.H5PayActivity"
            android:configChanges="orientation|keyboardHidden|navigation|screenSize"
            android:exported="false"
            android:screenOrientation="${demo_screen_orientation}"
            android:windowSoftInputMode="adjustResize|stateHidden"
            tools:replace="android:screenOrientation" />
</application>
```

**注意：第三方 Activity 需要根据接入方应用屏幕方向，手动配置横竖屏**

### 代码混淆配置
如果游戏有需要进行代码混淆，请不要混淆联编的jar包下的类，可以在`proguard.cfg`文件里追加以下配置排除SDK jar包中得类
```
# keep 4399 SDK
-keep class cn.m4399.single.** {*;}
-keep com.m4399.gamecenter.service.aidl.** {*;}

# 如果使用的是support的jar包依赖
# -keep android.support.v4.** {*;}

# keep 支付宝接口
-dontwarn com.alipay.**
-keep class com.alipay.** {*;}
```

# 接入流程
## 初始化
初始化推荐在游戏初始化过程中进行，析构函数则在游戏退出前执行。
```java
	
//初始化SDK
OperateConfig config = new OperateConfig()
        .debuggable(false) //发布游戏时，要设为false
        .orientation(getGameOrientation()) //设置SDK界面方向，应与游戏设置一直
        .supportExcess(true) // 设置“是否支持超出金额”默认值，也可以在每次充值的订单信息里设置
        .gameKey(getString(R.string.demo_label_game_key)) //换成实际游戏的game key
        .gameName(getString(R.string.demo_label_game_name)); //换成实际游戏的名字，原则上与游戏名字匹配
// MainActivity 是当前 Activity 对象	
SingleOperateCenter.init(MainActivity.this, config, mRechargeController.singleRechargeListener);
```

`是否支持处理超出部分金额`也可单独设置  
```java
public RechargeOrder supportExcess(boolean support);
```

> **关于是否支持处理超出部分金额**
> 
> - 由于用户选择的充值渠道不同，可能造成实际充值金额超出游戏下单时传入的金额
> - 如果游戏能够正确处理超出部分的金额，则本接口传入true
> - 如果无法支持处理超出部分的金额，则传入false，SDK将会根据传入金额自动隐藏无法满足充值金额的渠道
> 
> （例：开发者设置SupportExcess为false，充值时传入7元，此时4399一卡通中无7元面额的充值卡，此时4399一卡通的充值渠道将自动隐藏）。*SupportExcess*默认为false。

## 检查更新
SDK将检查后台是否有新版本游戏上线，如果有，则显示更新内容，并提示用户升级。该升级为`增量升级`，后台在提交新版游戏时自动制作差分包，更新时用户只需下载APK文件中新旧版本有差异的部分。相关更新内容和版本提交事宜，均须登录原创平台操作。  
4399SDK的增量升级分为  
- 全自动增量更新（无需操作，默认初始化完成）
- 自定义界面增量更新 （当游戏选择使用该策略时，上线前需与4399运营人员沟通）
自定义界面增量更新接入方法详见：[4399单机SDK增量升级说明](DeltaUpdate.md)  



## 充值
当用户需要充值时，可调用本接口启动充值中心界面。
```java
RechargeOrder order = 
	// 充值金额，整数，基本要求大于1，小于50000
	new RechargeOrder(1) 
        //是否支持超出金额，这里是一个独立的接口，只要在充值之前调用都有作用
        .supportExcess(true)
        //是否传入商品名，这会影响商品在充值界面的显示
        .commodity("周年庆礼包")
        // 透传参数，JSON 字符串
        .passthrough("测试参数");
SingleOperateCenter.recharge(mainActivity, order);
```

* `commodity`商品名称：最长不超过8个字符。 如果传入商品名，充值中心将直接显示改商品名称，如果充值金额大于下单时传入的`je`时，将显示商品名+XXX游戏币，相关游戏币的兑换比例在接入时提供给运营人员配置。如果未传入商品名，则直接显示XXX游戏币。

## 礼包兑换
- 调用方式
```java
SingleOperateCenter.validateGiftCode(MainActivity.this, new OnGiftCodeValidatedListener() {
	@Override
	public void onValidated(String code, String key) {
		// 接入方发放奖励，或进一步验证
	}
});
```
- 接口说明
```java		
    /**
     * 礼包兑换码验证接口
     *
     * SDK 将打开验证界面，进行接收、验证兑换码和其他交互
     * 验证成功后通知接入方相关信息
     *
     * @param activity Activity运行上下文
     * @param listener 验证成功的回调
     */
    public void validateGiftCode(Activity activity,OnGiftCodeValidatedListener listener) {
        new CodeValidateDialog(activity,listener).show();
    }

    public interface OnGiftCodeValidatedListener {
        /**
         * 礼包兑换码验证成功回调
         *
         * 接入方可以在此方法发放物品，也可以进一步对用户验证
         * <strong>若发放过程中可能出现错误，接入方需要自行处理</strong>
         *
         * @param code 礼包兑换码，礼包平台对礼包的标识，验证时使用
         * @param key 接入方对礼包的标识，在申请礼包活动时一并提交给礼包平台；
         *            验证成功时通知给接入方，发放时使用
         */
        void onValidated(String code, String key);
    }
```

## 激活码验证
[4399单机SDK激活模块API接入文档](GameActivation.md) 

## 析构
游戏退出时调用本接口，释放SDK资源以及保存相关数据。
```java
SingleOperateCenter.destroy();
```

## 显示SDK版本
```java
String version = SingleOperateCenter.getVersion();
```
