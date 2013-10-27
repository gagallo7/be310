<?php
/**
 * @file
 * This file contains all functions which need for calendar view.
 */

/**
 * View products in list.
 */
function spider_catalog_product_list($theme_id, $category_id, $show_category_details) {
  // Parameters for SpiderBox.
  drupal_add_js(array(
    'spider_catalog' => array(
      'delay' => 3000,
      'allImagesQ' => 0,
      'slideShowQ' => 0,
      'darkBG' => 1,
      'juriroot' => base_path() . drupal_get_path('module', 'spider_catalog'),
      'spiderShop' => 1,
    ),
    ),
    'setting');
  drupal_add_js(drupal_get_path('module', 'spider_catalog') . '/spiderBox/spiderBox.js');
  if (arg(0) == 'node' && is_numeric(arg(1))) {
    $nodeid = arg(1);
  }
  if (isset($_GET['page_num'])) {
    $page_num = $_GET['page_num'];
  }
  else {
    $page_num = 1;
  }
  if (isset($_POST['prod_name'])) {
    $prod_name = check_plain($_POST['prod_name']);
  }
  else {
    $prod_name = '';
  }
  if (isset($_POST['cat_id'])) {
    $cat_id = check_plain($_POST['cat_id']);
  }
  else {
    $cat_id = $category_id;
  }
  // If category has deleted, show all products.
  if (!db_query("SELECT id FROM {spider_catalog_product_categories} WHERE id=:id", array(':id' => $cat_id))->fetchField()) {
    $cat_id = 0;
  }
  if ($cat_id == 0) {
    $compare = '>=';
  }
  else {
    $compare = '=';
  }
  drupal_add_js(drupal_get_path('module', 'spider_catalog') . '/js/spider_catalog_common.js');
  drupal_add_css(drupal_get_path('module', 'spider_catalog') . '/css/spider_catalog_main.css');
  $category_list = db_query("SELECT id,name FROM {spider_catalog_product_categories} WHERE published=:published ORDER BY ordering", array(':published' => 1))->fetchAllKeyed();
  $cat_rows = db_query("SELECT * FROM {spider_catalog_product_categories} WHERE id" . $compare . ":id", array(':id' => $cat_id))->fetchObject();
  $params = db_query("SELECT * FROM {spider_catalog_params} WHERE id=:id", array(':id' => 1))->fetchObject();
  $theme = db_query("SELECT * FROM {spider_catalog_themes} WHERE id=:id", array(':id' => $theme_id))->fetchObject();
  if (!$theme) {
    $theme_id = 1;
    db_query("UPDATE {spider_catalog_form_table} SET theme=:theme WHERE vid=:vid", array(':theme' => 1, ':vid' => $nodeid));
  }
  $theme = db_query("SELECT * FROM {spider_catalog_themes} WHERE id=:id", array(':id' => $theme_id))->fetchObject();
  $prod_in_page = $theme->count_of_products_in_the_page_list;
  $rows = db_query("SELECT id FROM {spider_catalog_products} WHERE published=:published AND category_id" . $compare . ":category_id AND name LIKE :name ORDER BY ordering LIMIT " . (($page_num - 1) * $prod_in_page) . ", " . $prod_in_page, array(
    ':published' => 1,
    ':category_id' => $cat_id,
    ':name' => '%' . db_like($prod_name) . '%'))->fetchCol();
  $prod_count = db_query("SELECT COUNT(id) FROM {spider_catalog_products} WHERE category_id" . $compare . ":category_id AND name LIKE :name", array(':category_id' => $cat_id, ':name' => '%' . db_like($prod_name) . '%'))->fetchField();
  $permalink_for_sp_cat = url('spider_catalog/product_view', array('query' => array('theme_id' => $theme_id), 'absolute' => TRUE));
  $content = '';
  if ($params->product_ratings) {
    $rating_star_path = base_path() . drupal_get_path('module', 'spider_catalog') . '/images/star_images/star' . $theme->rating_star_design . '.png';
    $content .= '
      <style type="text/css">
        .star-rating { background: url("' . $rating_star_path . '") top left repeat-x !important; margin-top:0px;}
        .star-rating li a:hover { background: url("' . $rating_star_path . '") left bottom !important; }
        .star-rating li.current-rating { background: url("' . $rating_star_path . '") left center !important; }
        .star-rating1 { background: url("' . $rating_star_path . '") top left repeat-x !important;  margin-top:0px;}
        .star-rating1 li.current-rating	{ background: url("' . $rating_star_path . '") left center !important; }
      </style>';
  }
  if ($theme->rounded_corners) {
    $border_radius = '8px';
  }
  else {
    $border_radius = '0';
  }
  $content .= '
    <style type="text/css">
      #showPictureAnimatedTable tbody {
        border: none;
      }
      select.spidercataloginput {
        margin: 0 0 24px 0 !important;
        position: relative;
        top: -11px !important;
      }
      #CatalogSearchBox,
      .spidercatalogbutton,
      .spidercataloginput  {
        -moz-border-radius: ' . $border_radius . ' !important;
        -webkit-border-radius: ' . $border_radius . ' !important;
        border-radius: ' . $border_radius . ' !important;
        font-size: 10px !important;
        text-align: right !important;
      }
      #CatalogSearchBox {
        margin-bottom: 10px !important;
        text-align: right !important;
      }
      #productMainDiv,
      #productCartFull,
      .spidercatalogbutton,
      .spidercataloginput {
        -moz-border-radius: ' . $border_radius . ';
        -webkit-border-radius: ' . $border_radius . ';
        border-radius: ' . $border_radius . ';
      }
      #productMainDiv table,
      #productMainDiv td,
      #productMainDiv tr,
      #productMainDiv div,
      #productMainDiv tbody,
      #productMainDiv th {
        line-height: inherit;
      }
      #productMainDiv tr, td {
        padding: inherit !important;
      }
      #productMainDiv img {
        max-height: inherit;
        max-width: inherit;
      }
      #productMainDiv ul li,
      #productMainDiv ul,
      #productMainDiv li {
        list-style-type: none !important;
      }
      #productMainDiv #prodTitle {
        -moz-border-radius-topleft: ' . $border_radius . ';
        -moz-border-radius-topright: ' . $border_radius . ';
        -webkit-border-top-left-radius: ' . $border_radius . ';
        -webkit-border-top-right-radius: ' . $border_radius . ';
        border-top-left-radius: ' . $border_radius . ';
        border-top-right-radius: ' . $border_radius . ';
      }
      #productCartFull tr:first-child td:first-child {
        -moz-border-radius-topleft: ' . $border_radius . ';
        -webkit-border-top-left-radius: ' . $border_radius . ';
        border-top-left-radius: ' . $border_radius . ';
      }
      #productCartFull {
        margin: inherit;
      }
      #productCartFull table,
      #productCartFull tr,
      #productCartFull td {
        margin-bottom: 0px;
        margin-left: 0px;
        margin-right: 0px;
        margin-top: 0px;
        padding-bottom: 0px;
        padding-left: 0px;
        padding-right: 0px;
        padding-top: 0px;
      }
      #productCartFull {
        margin-bottom: 0px;
        margin-left: 0px;
        margin-right: 0px;
        margin-top: 0px;
        padding-bottom: 0px;
        padding-left: 0px;
        padding-right: 0px;
        padding-top: 0px;
      }
      #productCartFull tr:first-child td:last-child {
        -moz-border-radius-topright: ' . $border_radius . ';
        -webkit-border-top-right-radius: ' . $border_radius . ';
        border-top-right-radius: ' . $border_radius . ';
      }
      #productCartFull tr:last-child td:first-child {
        -moz-border-radius-bottomleft: ' . $border_radius . ';
        -webkit-border-bottom-left-radius: ' . $border_radius . ';
        border-bottom-left-radius: ' . $border_radius . ';
      }
      #productCartFull tr:last-child td:last-child {
        -moz-border-radius-bottomright: ' . $border_radius . ';
        -webkit-border-bottom-right-radius: ' . $border_radius . ';
        border-bottom-right-radius: ' . $border_radius . ';
      }
      #productCartFull td table td {
        -moz-border-radius: 0px !important;
        -webkit-border-radius: 0px !important;
        border-radius: 0px !important;
      }
      #productMainDiv td,
      #productMainDiv tr,
      #productMainDiv tbody,
      #productMainDiv div {
        background-color:inherit;
        color: inherit;
        line-height: inherit !important;
        opacity: inherit !important;
      }
      #category,
      #category table,
      #category tr,
      #category td,
      #category th,
      #category tbody {
        border: 0px;
        border-bottom: 0 !important;
        border-left: 0 !important;
        border-right: 0 !important;
        border-top: 0 !important;
        margin-bottom: 0px;
        margin-left: 0px;
        margin-right: 0px;
        margin-top: 0px;
        padding-bottom: 10px;
        padding-left: 10px;
        padding-right: 10px;
      }
      #productCartFull ul,
      #productCartFull li,
      #productCartFull a {
        background-color: inherit;
        list-style: none;
      }
      #spider_catalog_inform_table tr,
      #spider_catalog_inform_table td {
        text-align: left;
        vertical-align: top !important;
      }
      #spider_cat_price_tab {
        vertical-align: top !important;
      }
      #productMainDiv input,
      #productMainDiv textarea {
        -moz-border-radius: 3px;
        -moz-transition: all 0.2s linear 0s;
        -o-transition: all 0.2s linear 0s;
        -webkit-border-radius: 3px;
        -webkit-transition: all 0.2s linear;
        -webkit-transition-delay: 0s;
        border-radius: 3px;
        transition: all 0.2s linear 0s;
      }
      #productMainDiv textarea,
      #productMainDiv input[type="text"] {
        -moz-background-clip: padding;
        -moz-border-radius: 3px;
        -moz-box-sizing: border-box;
        -moz-box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.075);
        -webkit-appearance: none;
        -webkit-background-clip: padding;
        -webkit-border-radius: 3px;
        -webkit-box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.075);
        -webkit-box-sizing: border-box;
        background-clip: padding-box;
        background-color: white;
        border: 1px solid #cccccc;
        border-radius: 3px;
        box-sizing: border-box;
        box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.075);
        color: black;
        font-family: Arial, "Liberation Sans", FreeSans, sans-serif;
        font-size: 13px;
        height: 2.2em;
        margin: 0;
        outline: 0;
        padding: 3px 4px;
        text-align: left;
        vertical-align: top;
        *padding-top: 2px;
        *padding-bottom: 1px;
        *height: auto;
      }
      #productMainDiv textarea[disabled],
      #productMainDiv input[type="text"][disabled] {
        background-color: #eeeeee;
      }
      #productMainDiv textarea:focus,
      #productMainDiv input[type="text"]:focus {
        border-color: rgba(82, 168, 236, 0.8);
        outline: 0;
        outline: thin dotted \9;
        /* IE6-9 */
        -webkit-box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.075), 0 0 ' . $border_radius . ' rgba(82, 168, 236, 0.6);
        -moz-box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.075), 0 0 ' . $border_radius . ' rgba(82, 168, 236, 0.6);
        box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.075), 0 0 ' . $border_radius . ' rgba(82, 168, 236, 0.6);
      }
      #productMainDiv input[disabled],
      #productMainDiv textarea[disabled] {
        -moz-box-shadow: none;
        -moz-user-select: -moz-none;
        -khtml-user-select: none;
        -webkit-box-shadow: none;
        -webkit-user-select: none;
        box-shadow: none;
        color: #888888;
        cursor: default;
        user-select: none;
      }
      #productMainDiv input::-webkit-input-placeholder,
      #productMainDiv textarea::-webkit-input-placeholder {
        color: #888888;
      }
      #productMainDiv input:-moz-placeholder,
      #productMainDiv textarea:-moz-placeholder {
        color: #888888;
      }
      #productMainDiv input.placeholder_text,
      #productMainDiv textarea.placeholder_text {
        color: #888888;
      }
      #productMainDiv textarea {
        min-height: 80px;
        overflow: auto;
        padding: 5px;
        resize: vertical;
        width: 100%;
      }
      #productMainDiv optgroup {
        color: black;
        font-family: Arial, "Liberation Sans", FreeSans, sans-serif;
        font-style: normal;
        font-weight: normal;
      }
      #productMainDiv optgroup::-moz-focus-inner {
        border: 0;
        padding: 0;
      }
      #tdviewportheight div,
      #tdviewportheight td,
      #tdviewportheight{
        vertical-align: middle;
      }
    </style>';
  if ((($cat_id > 0) || ($cat_id != 0)) && ($show_category_details == 1)) {
    if ($params->category_details_width != '') {
      $category_details_width = 'width:' . $params->category_details_width . 'px;';
    }
    else {
      $category_details_width = '';
    }
    $imgurl = explode('#***#', $cat_rows->category_image_url);
    $content .= '
      <div id="productMainDiv" style="' . $category_details_width . 'border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';' . (($theme->text_size_product != '') ? ('font-size:' . $theme->text_size_product . 'px;') : '') . (($theme->text_color != '') ? ('color:#' . $theme->text_color . ';') : '') . (($theme->background_color != '') ? ('background-color:#' . $theme->background_color . ';') : '') . '">
        <div id="prodTitle" style="' . (($theme->title_color != '') ? ('color:#' . $theme->title_color . ';') : '') . (($theme->title_background_color != '') ? ('background-color:#' . $theme->title_background_color . ';') : '') . 'padding:10px;font-size:' . $theme->category_title_size . 'px;">' . $cat_rows->name . '</div>
        <table id="category" border="0" cellspacing="10" cellpadding="10">
          <tr>';
    if (($cat_rows->category_image_url != '') && ($imgurl[0] != '')) {
      $content .= '
            <td valign="top">
              <table cellpadding="0" cellspacing="5" border="0" style="margin:0px;width:' . ($theme->category_picture_width) . 'px;">
                <tr>
                  <td colspan="2" id="prod_main_picture_container" valign="top">
                    <div style="border:#CCCCCC solid 2px;padding:5px;background-color:white;">
                      <a href="' . $imgurl[0] . '" target="_blank" id="prod_main_picture_a" style="text-decoration:none;">
                        <div id="prod_main_picture" style=\'width:' . ($theme->category_picture_width) . 'px;height:' . ($theme->category_picture_height) . 'px; background:url("' . $imgurl[0] . '") center no-repeat; background-size: contain;\'>&nbsp;</div>
                      </a>
                    </div>
                  </td>
                </tr>
                <tr>
                  <td style="text-align:justify;">';
      $small_images_str = '';
      $small_images_count = 0;
      foreach ($imgurl as $img) {
        if ($img != '') {
          $small_images_str .= '
                    <a href="' . $img . '" target="_blank">
                      <img style="width:50px;heigth:50px" src="' . $img . '" vspace="0" hspace="0" onMouseOver=\'spider_catalog_prod_change_picture("' . $img . '",this,' . $theme->category_picture_width . ',' . $theme->category_picture_height . ');\' />
                    </a>';
          $small_images_count++;
        }
      }
      if ($small_images_count > 1) {
        $content .= $small_images_str;
      }
      else {
        $content .= '&nbsp;';
      }
      $content .= '</td>
                </tr>
              </table>
            </td>';
    }
    $content .= '
            <td valign="top">' . $cat_rows->description . '</td>
          </tr>
        </table>
      </div>';
  }
  $width_spider_main_table_lists = '';
  $a = 'form';
  $content .= '
      <div id="productMainDiv" style="' . $width_spider_main_table_lists . 'text-align:center">';
  // ($params["choose_category"] and !($params1['categories'] > 0)
  if ($params->search_by_category || $params->search_by_name) {
    $page_link = url('node/' . $nodeid, array('absolute' => TRUE));
    $content .= '
      <' . $a . ' action="' . $page_link . '" method="post" name="cat_form" id="cat_form_page_nav">
        <input type="hidden" name="page_num" value="1" />
        <div class="CatalogSearchBox">';
    if ($params->search_by_category && ($category_id == 0)) {
      $content .= '
          <span style="top:-11px; position: relative;">' . t('Choose Category') . '&nbsp</span>
          <select id="cat_id" name="cat_id" class="spidercataloginput" size="1" onChange="this.form.submit();">
            <option value="0">' . t('All') . '</option> ';
      foreach ($category_list as $key => $category_name) {
        if ($key == $cat_id) {
          $content .= '
            <option value="' . $key . '" selected="selected">' . $category_name . '</option>';
        }
        else {
          $content .= '
            <option value="' . $key . '" >' . $category_name . '</option>';
        }
      }
      $content .= '
          </select>';
    }
    if ($params->search_by_name) {
      if (isset($_POST['prod_name'])) {
        $prod_name = $_POST['prod_name'];
      }
      else {
        $prod_name = '';
      }
      $content .= '
          <br />' . t('Search') . '&nbsp;
          <input type="text" id="prod_name" name="prod_name" class="spidercataloginput" value="' . $prod_name . '">
          <input type="button" onclick="this.form.submit()" value="' . t('Go') . '" class="spidercatalogbutton" style="background-color:#' . $theme->buttons_background_color . '; color:#' . $theme->color_of_text_of_buttons . '; width:inherit;">
          <input type="button" value="' . t('Reset') . '" onClick="spider_catalog_cat_form_reset(this.form);" class="spidercatalogbutton" style="background-color:#' . $theme->buttons_background_color . '; color:#' . $theme->color_of_text_of_buttons . '; width:inherit;">';
    }
    $content .= '
        </div>
      </form>';
  }
  if (count($rows)) {
    $content .= '
      <table border="0" cellspacing="0" cellpadding="0" id="productCartFull" style="border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';border-bottom:none; border-right:none;' . (($theme->text_color != '') ? ('color:#' . $theme->text_color . ';') : '') . (($theme->background_color != '') ? ('background-color:#' . $theme->background_color . ';') : '') . '">
        <tr style="' . (($theme->title_color != '') ? ('color:#' . $theme->title_color . ';') : '') . '">';
    $parameters_exist = 0;
    foreach ($rows as $row) {
      $row_param = db_query("SELECT param FROM {spider_catalog_products} WHERE id=:id", array(':id' => $row))->fetchField();
      if ($row_param != "") {
        $parameters_exist++;
      }
    }
    $content .= '
          <TD style="border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';' . (($theme->title_background_color != '') ? ('background-color:#' . $theme->title_background_color . ';') : '') . 'border-top:none; border-left:none;">' . t('Product') . '</TD>
          <TD style="border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';' . (($theme->title_background_color != '') ? ('background-color:#' . $theme->title_background_color . ';') : '') . 'border-top:none; border-left:none;">' . t('Name');
    if ($params->product_ratings) {
      $content .= ' / ' . t('Rating');
    }
    $content .= '
          </TD>';
    if ($parameters_exist && $theme->show_parameters_list) {
      $content .= '
          <TD style="border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';' . (($theme->title_background_color != '') ? ('background-color:#' . $theme->title_background_color . ';') : '') . 'border-top:none; border-left:none;">' . t('Parameters') . '</TD>';
    }
    if ($theme->show_description_list) {
      $content .= '
          <TD style="border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';' . (($theme->title_background_color != '') ? ('background-color:#' . $theme->title_background_color . ';') : '') . 'border-top:none; border-left:none;">' . t('Description') . '</TD>';
    }
    if ($params->price || $params->market_price) {
      $content .= '
          <TD style="border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';' . (($theme->title_background_color != '') ? ('background-color:#' . $theme->title_background_color . ';') : '') . 'border-top:none; border-left:none;">' . t('Price') . '</TD>';
    }
    $content .= '
        </tr>';
  }
  foreach ($rows as $row) {
    $row_param = db_query("SELECT * FROM {spider_catalog_products} WHERE id=:id", array(':id' => $row))->fetchObject();
    $imgurl = explode('#***#', $row_param->image_url);
    $content .= '
        <tr>';
    if (!($row_param->image_url != '')) {
      $content .= '
          <td style="vertical-align: top !important; padding:0px;border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';border-top:none; border-left:none;">
            <img style="margin:10px; max-width:' . $theme->picture_width_list . 'px; max-height:' . $theme->picture_height_list . 'px" src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/no_image.jpg" />
          </td>';
    }
    else {
      $content .= '
          <td style="vertical-align: top !important; border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';border-top:none; border-left:none;">
            <a href="' . $imgurl[0] . '" target="_blank">
              <img style="border:#CCC solid 2px; margin:10px; max-width:' . $theme->picture_width_list . 'px; max-height:' . $theme->picture_height_list . 'px" src="' . $imgurl[0] . '" />
            </a>
          </td>';
    }
    $content .= '
          <td style="font-size:12px; border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';border-top:none; border-left:none;">
            <a href="' . $permalink_for_sp_cat . '&product_id=' . $row_param->id . '&page_num=' . $page_num . '&back=' . $nodeid . '" style="' . (($theme->hyperlink_color != '') ? ('color:#' . $theme->hyperlink_color . ';') : '') . '">' . $row_param->name . '</a>';
    if ($theme->show_category_list) {
      if ($row_param->category_id == 0) {
        $category_title = t('Uncategorized');
      }
      else {
        $category_title = $category_list[$row_param->category_id];
      }
      $content .= '
            <br />
            <div style="margin:10px;">
              <b>' . t('Category:') . '</b>&nbsp;&nbsp;&nbsp;
              <span style="' . (($theme->color_of_parameter_values != '') ? ('color:#' . $theme->color_of_parameter_values . ';') : '') . '" id="cat_' . $row_param->id . '">' . $category_title . '</span>
            </div>';
    }
    $id = $row_param->id;
    $ratings[$id] = db_query("SELECT AVG(vote_value) as rating FROM {spider_catalog_product_votes} WHERE product_id=:product_id", array(':product_id' => $id))->fetchField();
    $ratings_id = drupal_substr($ratings[$id], 0, 3);
    if ($params->product_ratings) {
      $voted[$id] = db_query("SELECT vote_value FROM {spider_catalog_product_votes} WHERE product_id=:product_id AND remote_ip=:remote_ip", array(
        ':product_id' => $id,
        ':remote_ip' => ip_address(),
      ))->fetchField();
      if ($voted[$id] == 0) {
        if ($ratings[$id] == 0) {
          $title = t('Not rated Yet.');
        }
        else {
          $title = drupal_substr($ratings[$id], 0, 3);
        }
        $rating = $ratings_id * 25;
        $content .= '
            <div id="voting' . $id . '" style="height:50px; padding:10px;">
              <ul class="star-rating">
                <li class="current-rating" id="current-rating" style="width:' . $rating . 'px"></li>
                <li>
                  <a href="#" onclick=\'spider_catalog_vote(1, ' . $id . ', "voting' . $id . '", "' . t('Rated.') . '", "' . url('spider_catalog/star_rate', array('query' => array('ex' => 1), 'absolute' => TRUE)) . '"); return false;\'	title="' . $title . '" class="one-star">1</a>
                </li>
                <li>
                  <a href="#" onclick=\'spider_catalog_vote(2, ' . $id . ', "voting' . $id . '", "' . t('Rated.') . '", "' . url('spider_catalog/star_rate', array('query' => array('ex' => 1), 'absolute' => TRUE)) . '"); return false;\'	title="' . $title . '" class="two-stars">2</a>
                </li>
                <li>
                  <a href="#" onclick=\'spider_catalog_vote(3, ' . $id . ', "voting' . $id . '", "' . t('Rated.') . '", "' . url('spider_catalog/star_rate', array('query' => array('ex' => 1), 'absolute' => TRUE)) . '"); return false;\'	title="' . $title . '" class="three-stars">3</a>
                </li>
                <li>
                  <a href="#" onclick=\'spider_catalog_vote(4, ' . $id . ', "voting' . $id . '", "' . t('Rated.') . '", "' . url('spider_catalog/star_rate', array('query' => array('ex' => 1), 'absolute' => TRUE)) . '"); return false;\'	title="' . $title . '" class="four-stars">4</a>
                </li>
                <li>
                  <a href="#" onclick=\'spider_catalog_vote(5, ' . $id . ', "voting' . $id . '", "' . t('Rated.') . '", "' . url('spider_catalog/star_rate', array('query' => array('ex' => 1), 'absolute' => TRUE)) . '"); return false;\'	title="' . $title . '" class="five-stars">5</a>
                </li>
              </ul>
            </div>';
      }
      else {
        if ($ratings[$id] == 0) {
          $title = t('Not rated Yet.');
        }
        else {
          $title = t('Rating:') . '&nbsp;' . $ratings_id . '&nbsp;&nbsp;&nbsp;&nbsp;&#013;' . t('You have already rated this product.');
        }
        $content .= '
            <div id="voting' . $id . '" style="height:50px; padding:10px;">
              <ul class="star-rating1">
                <li class="current-rating" id="current-rating" style="width:' . ($ratings_id * 25) . 'px"></li>
                <li><a title="' . $title . '" class="one-star">1</a></li>
                <li><a title="' . $title . '" class="two-stars">2</a></li>
                <li><a title="' . $title . '" class="three-stars">3</a></li>
                <li><a title="' . $title . '" class="four-stars">4</a></li>
                <li><a title="' . $title . '" class="five-stars">5</a></li>
              </ul>
            </div>';
      }
    }
    $content .= '
          </td>';
    if ($parameters_exist && $theme->show_parameters_list) {
      $content .= '
          <td style="font-size:12px; padding:0px;border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';border-top:none; border-left:none;vertical-align:top;">
            <table id="spider_catalog_inform_table" border="0" cellspacing="0" cellpadding="0" width="100%">';
      $par_data = explode("#@@@#", $row_param->param);
      for ($j = 0; $j < count($par_data); $j++) {
        if ($par_data[$j] != '') {
          $par1_data = explode("@@:@@", $par_data[$j]);
          $par_values = explode("#***#", $par1_data[1]);
          $count_of_par = 0;
          for ($k = 0; $k < count($par_values); $k++) {
            if ($par_values[$k] != "") {
              $count_of_par++;
            }
          }
          $bgcolor = ((!($j % 2)) ? (($theme->parameters_background_ccolor_2 != '') ? ('background-color:#' . $theme->parameters_background_ccolor_2 . ';') : '') : (($theme->parameters_background_ccolor_1 != '') ? ('background-color:#' . $theme->parameters_background_ccolor_1 . ';') : ''));
          if ($count_of_par != 0) {
            $content .= '
              <tr style="' . $bgcolor . 'text-align:left">
                <td><b>' . stripslashes($par1_data[0]) . ':</b></td>
                <td style="' . (($theme->text_size_list != '') ? ('font-size:' . $theme->text_size_list . 'px;') : '') . $bgcolor . (($theme->color_of_parameter_values != '') ? ('color:#' . $theme->color_of_parameter_values . ';') : '') . /*'width:' . $params['parameters_select_box_width'] . 'px;*/'">
                  <ul class="spidercatalogparamslist">';
            for ($k = 0; $k < count($par_values); $k++) {
              if ($par_values[$k] != "") {
                $content .= '
                    <li>' . stripslashes($par_values[$k]) . '</li>';
              }
            }
            $content .= '
                  </ul>
                </td>
              </tr>';
          }
        }
      }
      $content .= '
            </table>
          </td>';
    }
    if ($theme->show_description_list) {
      $description = $row_param->short_description;
      $content .= '
          <td style="border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';border-top:none; border-left:none;padding:10px">
            <div id="prodDescription" style="font-size:12px;">' . $description . ' </div>
            <div id="prodMore"  style="font-size:12px;">
              <a href="' . $permalink_for_sp_cat . '&product_id=' . $id . '&page_num=' . $page_num . '&back=' . $nodeid . '" style="' . (($theme->hyperlink_color != '') ? ('color:#' . $theme->hyperlink_color . ';') : '') . '">' . t('More') . '</a>
            </div>
          </td>';
    }
    if ($params->price || $params->market_price) {
      $content .= '
          <td id="spider_cat_price_tab" style="border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';border-top:none; border-left:none;">';
      if ($params->price && ($row_param->cost != 0) && ($row_param->cost != '')) {
        $content .= '
            <div id="prodCost" style="font-size:' . $theme->price_size_list . 'px;color:#' . $theme->price_color . ';">' . (($params->currency_symbol_position == 0) ? ($params->currency_symbol) : '') . '&nbsp;' . $row_param->cost . '&nbsp;' . (($params->currency_symbol_position == 1) ? $params->currency_symbol : '') . '</div>';
      }
      if ($params->market_price && ($row_param->market_cost != 0) && ($row_param->market_cost != '')) {
        $content .= '
            <div id="prodCost" style="font-size:' . ($theme->price_size_list / 1.7) . 'px;">' . t('Market Price:') . ' 
              <span style=" text-decoration:line-through;color:#' . $theme->price_color . ';"> ' . (($params->currency_symbol_position == 0) ? ($params->currency_symbol) : '') . '&nbsp;' . $row_param->market_cost . '&nbsp;' . (($params->currency_symbol_position == 1) ? $params->currency_symbol : '') . '</span>
            </div>';
      }
      $content .= '
          </td>';
    }
    $content .= '
        </tr>';
  }
  if (count($rows)) {
    $content .= '
      </table>';
  }
  drupal_add_js('
    function spider_catalog_submit_catal(page_link) {
      if (document.getElementById("cat_form_page_nav")) {
        document.getElementById("cat_form_page_nav").setAttribute("action", page_link);
        document.getElementById("cat_form_page_nav").submit();
      }
      else {
        window.location.href = page_link;
      }
    }', array('type' => 'inline'));
  $content .= '
      <div id="spidercatalognavigation" style="text-align:center;">';
  if ($prod_count > $prod_in_page and $prod_in_page > 0) {
    $r = ceil($prod_count / $prod_in_page);
    $navstyle = (($theme->text_size_list != '') ? ('font-size:' . $theme->text_size_list . 'px !important;') : 'font-size:12px !important;') . (($theme->text_color != '') ? ('color:#' . $theme->text_color . ' !important;') : '');
    $link = url('node/' . $nodeid, array('query' => array('page_num' => ''), 'absolute' => TRUE));
    if ($page_num > 5) {
      $link = url('node/' . $nodeid, array('query' => array('page_num' => '1'), 'absolute' => TRUE));
      $content .= '
        &nbsp;&nbsp;
        <a href=\'javascript:spider_catalog_submit_catal("' . $link . '")\' style="' . $navstyle . '">' . t('First') . '</a>&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;...&nbsp;';
    }
    if ($page_num > 1) {
      $link = url('node/' . $nodeid, array('query' => array('page_num' => ($page_num - 1)), 'absolute' => TRUE));
      $content .= '
        &nbsp;&nbsp;
        <a href=\'javascript:spider_catalog_submit_catal("' . $link . '")\' style="' . $navstyle . '">' . t('Prev') . '</a>&nbsp;&nbsp;';
    }
    for ($i = $page_num - 4; $i < ($page_num + 5); $i++) {
      if ($i <= $r && $i >= 1) {
        $link = url('node/' . $nodeid, array('query' => array('page_num' => $i), 'absolute' => TRUE));
        if ($i == $page_num) {
          $content .= '
        <span style="font-weight:bold;color:#000000">&nbsp;' . $i . '&nbsp;</span>';
        }
        else {
          $content .= '
        <a href=\'javascript:spider_catalog_submit_catal("' . $link . '")\' style="' . $navstyle . '">&nbsp;' . $i . '&nbsp;</a>';
        }
      }
    }
    if ($page_num < $r) {
      $link = url('node/' . $nodeid, array('query' => array('page_num' => ($page_num + 1)), 'absolute' => TRUE));
      $content .= '
        &nbsp;&nbsp;
        <a href=\'javascript:spider_catalog_submit_catal("' . $link . '")\' style="' . $navstyle . '">' . t('Next') . '</a>&nbsp;&nbsp;';
    }
    if (($r - $page_num) > 4) {
      $link = url('node/' . $nodeid, array('query' => array('page_num' => $r), 'absolute' => TRUE));
      $content .= '
        &nbsp;...&nbsp;&nbsp;&nbsp;
        <a href=\'javascript:spider_catalog_submit_catal("' . $link . '")\' style="' . $navstyle . '">' . t('Last') . '</a>';
    }
  }
  $content .= '
      </div>
    </div>';
  drupal_add_js('var SpiderCatOFOnLoad = window.onload;window.onload = SpiderCatAddToOnload;', array('type' => 'inline', 'scope' => 'footer'));
  return $content;
}
