{*
 * $Revision: 1 $
 * If you want to customize this file, do not edit it directly since future upgrades
 * may overwrite it.  Instead, copy it into a new directory called "local" and edit that
 * version.  Gallery will look for that file first and use it if it exists.
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Embed Video Settings"} </h2>
</div>

<input type="hidden" name="{g->formVar var="form[variable][type]"}" />

<table><tr valign="top"><td>

<div class="gbBlock">
  <h3>{g->text text="Defaults"}</h3>
  <p>{g->text text="These variables provide default values for users on your site. Users will be able to override these defaults by making changes in the user interface of the applets, or by changing their local defaults file."}</p>
{if empty($form.embedvideodefaultVariables)}
  <p>{g->text text="You have no default variables"}</p>
{else}

  <table class="gbDataTable">
    <tr>
      <th> {g->text text="Variable"} </th>
      <th> {g->text text="Action"} </th>
    </tr>
    {foreach from=$form.embedvideodefaultVariables item=variable}
      <tr class="{cycle values="gbEven,gbOdd"}">
	<td>{$variable}</td>
	<td><a href="{g->url arg1="controller=embedvideo.EmbedVideoSiteAdmin"
	   arg2="form[action][delete]=1"
	   arg3=$variable|regex_replace:"/^(.*?)=.*$/":"form[delete][variable]=\\1"
	   arg4="form[variable][type]=default" arg5="mode=variables"}">
	   {g->text text="Delete"}</a></td>
      </tr>
    {/foreach}
  </table>
{/if}
</div>

<div class="gbBlock">
  <h4> {g->text text="Add a new default variable"} </h4>

  {if isset($form.error.default)}
  <div class="giError">
    {g->text text="You must enter a variable name and value"}
  </div>
  {/if}

  {g->text text="New variable"}<br/>
  <input type="text" name="{g->formVar var="form[default][name]"}" /> =
  <input type="text" name="{g->formVar var="form[default][value]"}" />
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][add]"}" value="{g->text text="Add variable"}"
   onclick="javascript:this.form['{g->formVar var="form[variable][type]"}'].value='default';this.form.submit();" />
</div>

<div class="gbBlock">
  <h3>{g->text text="Overrides"}</h3>
  <p>{g->text text="These variables override any other values for applets users execute on your site. Users will not be able to change these values."}</p>
{if empty($form.embedvideooverrideVariables)}
  <p>{g->text text="You have no override variables"}</p>
{else}

  <table class="gbDataTable">
    <tr>
      <th> {g->text text="Variable"} </th>
      <th> {g->text text="Action"} </th>
    </tr>
    {foreach from=$form.embedvideooverrideVariables item=variable}
      <tr class="{cycle values="gbEven,gbOdd"}">
	<td>{$variable}</td>
	<td><a href="{g->url arg1="controller=embedvideo.EmbedVideoSiteAdmin"
	   arg2="form[action][delete]=1"
	   arg3=$variable|regex_replace:"/^(.*?)=.*$/":"form[delete][variable]=\\1"
	   arg4="form[variable][type]=override" arg5="mode=variables"}">
	   {g->text text="Delete"}</a></td>
      </tr>
    {/foreach}
  </table>
{/if}
</div>

<div class="gbBlock">
  <h4> {g->text text="Add a new override variable"} </h4>

  {if isset($form.error.override)}
  <div class="giError">
    {g->text text="You must enter a variable name and value"}
  </div>
  {/if}

  {g->text text="New variable"}<br/>
  <input type="text" name="{g->formVar var="form[override][name]"}" /> =
  <input type="text" name="{g->formVar var="form[override][value]"}" />
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][add]"}" value="{g->text text="Add variable"}"
   onclick="javascript:this.form['{g->formVar var="form[variable][type]"}'].value='override';this.form.submit();" />
</div>


</td><td>
<div class="gbBlock">
  <h3>{g->text text="Help"}</h3>
  <p>{g->text text="Here are a selection of variables that affect video embedding."}</p>
  <table class="gbDataTable">
    <tr><th>{g->text text="variable"}</th><th>{g->text text="values"}</th><th>{g->text text="help"}</th></tr>

    <tr class="gbEven"><td>useInternalFlvPlayer</td><td>true/false</td>
      <td>{g->text text="instructs the module to use Gallery's built-in flv player when embedding links to .flv files"}</td></tr>

    <tr class="gbOdd"><td>externalFlvPlayer</td><td>URL to external flv player (.swf)</td>
      <td>{g->text text="instructs the module to use an external flv player when embedding links to .flv files"}</td></tr>

    <tr class="gbEven"><td>externalFlvPlayerVars</td><td>flashvars= options for external flash player</td>
      <td>{g->text text="provides the module with the flashvars to use for the external flash player"}</td></tr>

    <tr class="gbEven"><td>youtubeShowRelated</td><td>true/false</td>
      <td>{g->text text="specify whether you want to show the related video links in the youtube embedded player"}</td></tr>

    <tr class="gbOdd"><td>useRemoteSize</td><td>true/false</td>
      <td>{g->text text="use the remote video size instead of the width and height specified here"}</td></tr>

    <tr class="gbEven"><td>autoStart</td><td>true/false</td>
      <td>{g->text text="specify if the embedded video players should autostart the videos or not"}</td></tr>

    <tr class="gbEven"><td>allowFullScreen</td><td>true/false</td>
      <td>{g->text text="specify if the embedded video player should be allowed to playback in full screen mode or not (youtube only)"}</td></tr>

    <tr class="gbOdd"><td>width</td><td>320</td>
      <td>{g->text text="specify the width dimension of the embedded video player"}</td></tr>

    <tr class="gbEven"><td>height</td><td>240</td>
      <td>{g->text text="specify the height dimension of the embedded video player"}</td></tr>

    <tr class="gbOdd"><td>debugOutput</td><td>true/false</td>
      <td>{g->text text="turn on module debug output"}</td></tr>

    <tr class="gbEven"><td>flvThumbnail</td><td>URL path</td>
      <td>{g->text text="URL path to an alternate jpg to use as a thumbnail for all directly linked/embedded flv files."}</td></tr>

    <tr class="gbOdd"><td>watermarkVideos</td><td>true/false</td>
      <td>{g->text text="specify whether you want to apply a watermark to the video thumbnails when they are added."}</td></tr>

    <tr class="gbEven"><td>watermarkImage</td><td>URL or local path</td>
      <td>{g->text text="URL or local path to an alternate image to use as a watermark for all video thumbnails."}</td></tr>

    <tr class="gbOdd"><td>watermarkAlignment</td><td>alignment</td>
      <td>{g->text text="Specify the watermark alignment: top-left, top, top-right, left, center, right, bottom-left, bottom, bottom-right"}</td></tr>

    <tr class="gbEven"><td>wordwrapSummary</td><td>0 - N</td>
      <td>{g->text text="If non-zero, sets the maximum line width when displaying the item's summary"}</td></tr>

    <tr class="gbOdd"><td>wordwrapDescription</td><td>0 - N</td>
      <td>{g->text text="If non-zero, sets the maximum line width when displaying the item's description"}</td></tr>

    <tr class="gbEven"><td>centerVideo</td><td>true/false</td>
      <td>{g->text text="If set to true, video playback will be centered on the photo page"}</td></tr>


  </table>

</div>
</td></tr></table>
