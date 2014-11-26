# 4399运营SDK增量升级说明
## 升级方式

1. 全自动升级。 所有升级处理逻辑及界面显示，都由sdk完成。
2. 自定义界面升级。 游戏方可以拿到升级信息及下载进度，自定义升级界面。

## 全自动升级
当游戏初始化时，SDK将检查后台是否有新版本游戏上线，如果有，则显示更新内容，并提示用户升级。后台在提交新版游戏时自动制作差分包，更新时用户只需下载APK文件中新旧版本有差异的部分。相关更新内容和版本提交事宜，请联系4399相关运营对接人员。全自动更新无需配置和代码接入，SDK在初始化时自动检查更新。

## 自定义升级界面
如果游戏需要自定义更新界面，需告知4399运营人员，由后台关闭全自动更新开关。由游戏方自主接入更新接口  

### 1.1 检查版本更新
```java
/**
 * @param MainActivity.this Activity上下文
 * @param OnCheckFinishedListener 请求结果回调，监听器用于返回请求新版本的信息
 */
OperateCenter.getInstance().doCheck(MainActivity.this, new OnCheckFinishedListener() {
	/**
 	 * 检查新版本后，返回的新版本信息
 	 * @param checkResultInfo 新版本信息的对象 里面包括服务端返回的升级信息
 	 */
	@Override
	public void onCheckResponse(UpgradeInfo upgradeInfo) {
		// 接入者可以在这里显示检查结果，检查结果由UpgradeInfo封装
		showCheckResult(upgradeInfo);
	}
});
```

`UpgradeInfo` 封装了检查版本后，必要的可显示给用户的信息，其中公有方法：

| 方法名 | 说明 |
|-------|------|
|getResultCode	 |获取检查结果状态码
|getResultMsg    |获取检查结果提示信息|
|getUpgradeType  |获取更新类型， 1为增量更新，0为全量更新|
|getVersionName	 |获取线上最新版本VersionName|
|getVersionCode  |获取线上最新版本Versioncode|
|getUpgradeMsg	 |获取线上最版本的更新内容提示，如新增什么活动，调整什么装备属性等|
|getUpgradeTime  |获取线上最新版本发布时间|
|getUpgradeSize  |获取本次更新需要下载包的大小|
|getNewApkSize   |获取线上最新版本APK大小|
|haveLocalSrc    |本地是否有已经下载好的更新包，true表示有，false表示没有|
|isCompel        |判断是否强制升级|

### 1.2 显示检查结果
```java
private void showCheckResult(UpgradeInfo info) {
	int code = info.getResultCode();

	if (code == UpgradeInfo.APK_CHECK_NO_UPDATE) {
		// 没有更新，这里可以选择不提示
	}  else if (code == UpgradeInfo.APK_CHECK_NEED_UPDATE) {
		// 有更新内容
		if (info.haveLocalSrc()) {
			// 本地已有下载好的更新包，显示“立即安装”, 即开始安装
		} else {
			// 本地没有下载好的更新包
			if (info.isCompel()) {
				// 强制更新
			} else {
				// 非强制更新
			}
		}
	
	} else {
		// 检查更新失败，根据需要决定是否显示失败信息
	}
}
```

### 1.3 下载更新包
#### 接口
```java
/**
 * @param MainActivity.this Actvity上下文
 * @param OnDownloadListener 下载监听器，返回下载过程各个阶段的信息
 */
OperateCenter.getInstance().doDownload(MainActivity.this, new OnDownloadListener() {

	@Override
	public void onDownloadStart() {
		// 开始下载
	}
	
	/**
	 * @param progress 已下载的字节数，单位B
	 * @param max	   总的字节数， 单位B
	 */
	@Override
	public void onDownloadProgress(long progress, long max) {
		// 正在下载
	}
	
	@Override
	public void onDownloadSuccess() {
		// 下载成功, 可以提醒安装
	}
	
	/**
	 * @param resultCode, 下载失败时的HttpCode，由网络层返回
	 * @param eMsg, 下载失败时的出错信息，由网络层返回（英文字符）
	 */
	@Override
	public void onDownloadFail(int resultCode, String eMsg) {
		// 下载失败
	}
```
### 1.4 安装更新包
```java
/*
 * 下载成功后，或者本地已有下载好的更新包时，可以调用此函数
 * SDK会更加是增量更新还是全量更新采取不同的安装方式
 */
OperateCenter.getInstance().doInstall();
```

