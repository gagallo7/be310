<?php
/**
 * @file
 * This file contains all functions which need for calendar view.
 */

/**
 * View single product.
 */
function spider_catalog_single_product($product_id, $theme_id) {
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
  if (isset($_GET['rev_page'])) {
    $rev_page = $_GET['rev_page'];
  }
  else {
    $rev_page = 1;
  }
  if (isset($_GET['theme_id'])) {
    $theme_id = check_plain($_GET['theme_id']);
  }
  if (isset($_GET['page_num'])) {
    $page_num = check_plain($_GET['page_num']);
  }
  else {
    $page_num = 1;
  }
  $page_url = '&page_num=' . $page_num;
  if (isset($_GET['back'])) {
    $nod = check_plain($_GET['back']);
    $back_url = '&back=' . $nod;
  }
  else {
    $back_url = '';
  }
  if (!isset($_GET['product_id'])) {
    if (arg(0) == 'node' && is_numeric(arg(1))) {
      $nodeid = arg(1);
    }
    $link_url = url('node/' . $nodeid, array('query' => array('example' => 1), 'absolute' => TRUE));
    $theme = db_query("SELECT * FROM {spider_catalog_themes} WHERE id=:id", array(':id' => $theme_id))->fetchObject();
    if (!$theme) {
      $theme_id = 1;
      db_query("UPDATE {spider_catalog_form_table} SET theme=:theme WHERE vid=:vid", array(':theme' => 1, ':vid' => $nodeid));
    }
  }
  else {
    $product_id = check_plain($_GET['product_id']);
    $link_url = url('spider_catalog/product_view', array(
      'query' => array(
        'theme_id' => $theme_id,
        'product_id' => $product_id,
      ), 'absolute' => TRUE)) . $back_url;
  }
  if (isset($_POST['full_name']) && isset($_POST['message_text']) && isset($_POST['code'])) {
    $full_name = check_plain($_POST['full_name']);
    $message_text = check_plain($_POST['message_text']);
    $code = check_plain($_POST['code']);
    if ($code != '' && $full_name != '' && $code == $_SESSION['spider_catalog_captcha_code']) {
      $insert = db_insert('spider_catalog_product_reviews')->fields(array(
        'name' => $full_name,
        'content' => $message_text,
        'product_id' => $product_id,
        'remote_ip' => ip_address(),
      ))->execute();
      if (!$insert) {
        drupal_add_js('alert(Drupal.t("Error")); window.history.go(-1);', array('type' => 'inline'));
        exit();
      }
    }
  }
  drupal_add_js(drupal_get_path('module', 'spider_catalog') . '/js/spider_catalog_common.js');
  drupal_add_css(drupal_get_path('module', 'spider_catalog') . '/css/spider_catalog_main.css');
  $params = db_query("SELECT * FROM {spider_catalog_params} WHERE id=:id", array(':id' => 1))->fetchObject();
  $theme = db_query("SELECT * FROM {spider_catalog_themes} WHERE id=:id", array(':id' => $theme_id))->fetchObject();
  $row = db_query("SELECT * FROM {spider_catalog_products} WHERE id=:id", array(':id' => $product_id))->fetchObject();
  // If product deleted.
  if (!$row) {
    $content = '<div class="messages error">' . t('The chosen Product was deleted. Select another Product.') . '</div>';
    return $content;
  }
  $category_name = db_query("SELECT name FROM {spider_catalog_product_categories} WHERE id=:id", array(':id' => $row->category_id))->fetchField();
  $value = db_query("SELECT id,vote_value FROM {spider_catalog_product_votes} WHERE product_id=:product_id AND remote_ip=:remote_ip", array(':product_id' => $product_id, ':remote_ip' => ip_address()))->fetchAllKeyed();
  $voted = count($value);
  $row1 = db_query("SELECT AVG(vote_value) as rating FROM {spider_catalog_product_votes} WHERE product_id=:product_id", array(':product_id' => $product_id))->fetchField();
  $rating = drupal_substr($row1, 0, 3);
  $reviews_perpage = $theme->number_of_reviews_per_page;
  $reviews_rows = db_query("SELECT id FROM {spider_catalog_product_reviews} WHERE product_id=:product_id ORDER BY id DESC LIMIT " . (($rev_page - 1) * $reviews_perpage) . ", " . $reviews_perpage, array(':product_id' => $product_id))->fetchCol();
  $content = '
    <div>';
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
        top: -11px !important;
        position: relative;
      }
      #productMainDiv {
        -moz-border-radius: ' . $border_radius . ' !important;
        -webkit-border-radius: ' . $border_radius . ' !important;
        border-radius: ' . $border_radius . ' !important;
      }
      .spidercatalogbutton, .spidercataloginput {
        -moz-border-radius: ' . $border_radius . ' !important;
        -webkit-border-radius: ' . $border_radius . ' !important;
        border-radius: ' . $border_radius . ' !important;
        font-size: 10px !important;
      }
      #productMainDiv td, #productMainDiv tr, #productMainDiv tbody,  #productMainDiv div {
        background-color: inherit;
        color: inherit;
        line-height: inherit !important;
        opacity: inherit !important;
      }
      #productMainDiv {
        min-width: 540px;
      }
      #productMainDiv #prodTitle {
        -moz-border-radius-topleft: ' . $border_radius . ';
        -moz-border-radius-topright: ' . $border_radius . ';
        -webkit-border-top-left-radius: ' . $border_radius . ';
        -webkit-border-top-right-radius: ' . $border_radius . ';
        border-top-left-radius: ' . $border_radius . ';
        border-top-right-radius: ' . $border_radius . ';
        font-size: 10px;
      }
      #spider_caltalog_top_table table, #spider_caltalog_top_table th,  #spider_caltalog_top_table tbody,  #spider_caltalog_top_table tr,  #spider_caltalog_top_table td , #spider_caltalog_top_table body {
        font-weight: bold;
        line-height: inherit;
        text-align: left;
        vertical-align: middle;
      }
      #spider_caltalog_top_table {
        border-collapse: inherit;
        border: inherit !important;
        margin: inherit;
        padding: inherit;
      }
      #spider_caltalog_top_table ul, #spider_caltalog_top_table li, #spider_caltalog_top_table a {
        background-color: inherit;
        list-style: inherit;
      }
      #prodMiddle tr td, #prodMiddle td, #prodMiddle tr, #prodMiddle table, #prodMiddle table,#prodMiddle {
        border: 0 !important;
      }
      #spider_catalog_informaton_teble ul, #spider_catalog_informaton_teble li {
        overflow-x: inherit !important;
        overflow-y: inherit !important;
        width:inherit !important;
      }
      #spider_catalog_informaton_teble, #spider_catalog_informaton_teble table,  #spider_catalog_informaton_teble tbody {
        border-collapse: inherit;
        float: right;
        width: inherit !important;
      }
      #spider_catalog_informaton_teble td {
        text-align: left;
      }
      #tdviewportheight div, #tdviewportheight td, #tdviewportheight {
        vertical-align: middle;
      }
      #spider_catalog_image_table {
        border-spacing: inherit;
      }
      #prodTitle {
        font-family: inherit;
      }
      input,
      textarea {
        -o-transition: all 0.2s linear 0s;
        -moz-border-radius: 3px;
        -moz-transition: all 0.2s linear 0s;
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
        -moz-box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.075);
        -moz-box-sizing: border-box;
        -webkit-appearance: none;
        -webkit-background-clip: padding;
        -webkit-border-radius: 3px;
        -webkit-box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.075);
        -webkit-box-sizing: border-box;
        background-clip: padding-box;
        background-color: white;
        border-radius: 3px;
        box-sizing: border-box;
        box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.075);
        border: 1px solid #cccccc;
        color: black;
        outline: 0;
        margin: 0;
        padding: 3px 4px;
        text-align: left;
        font-size: 13px;
        font-family: Arial, "Liberation Sans", FreeSans, sans-serif;
        height: 2.2em;
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
        -moz-box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.075), 0 0 ' . $border_radius . ' rgba(82, 168, 236, 0.6);
        -webkit-box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.075), 0 0 ' . $border_radius . ' rgba(82, 168, 236, 0.6);
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
      .rating_stars ul,
      .rating_stars li,
      .rating_stars ul li {
        list-style-type:none !important;
      }
      #spider_catalog_captcha_table span,
      #spider_catalog_captcha_table,
      #spider_catalog_captcha_table tr,
      #spider_catalog_captcha_table td,
      #spider_catalog_captcha_table img,
      #spider_catalog_captcha_table tr td {
        border:0 !important;
        padding:2px;
        vertical-align:middle !important;
      }
      #message_text {
        max-width:inherit !important;
        min-height:100px;
      }
      #full_name {
        max-width:inherit !important;
      }
    </style>';
  if (isset($_GET['back'])) {
    $nod = check_plain($_GET['back']);
    $content .= '
    <span id="back_to_spidercatalog_button">
      <a href="' . url('node/' . $nod, array('query' => array('example' => 1), 'absolute' => TRUE)) . $page_url . '" >' . t('Back to Catalog') . '</a>
    </span>';
  }
  if ($params->category_details_width != '') {
    $width_spider_cat_prod_page = "width:" . $params->category_details_width . "px; ";
  }
  else {
    $width_spider_cat_prod_page = '';
  }
  $content .= '
    <div id="productMainDiv" style="' . $width_spider_cat_prod_page . ' border-width:' . $theme->border_width . 'px; border-color:#' . $theme->border_color . '; border-style:' . $theme->border_style . ';' . (($theme->text_size != '') ? ('font-size:' . $theme->text_size_product . 'px;') : '') . (($theme->text_color != '') ? ('color:#' . $theme->text_color . ';') : '') . (($theme->background_color != '') ? ('background-color:#' . $theme->background_color . ';') : '') . '">
      <div id="prodTitle" style="' . (($theme->title_color != '') ? ('color:#' . $theme->title_color . ';') : '') . (($theme->title_background_color != '') ? ('background-color:#' . $theme->title_background_color . ';') : '') . 'padding:0px;">
        <table id="spider_caltalog_top_table" border="0" cellspacing="0" cellpadding="0" width="100%">
          <tr>
            <td style="border:none; padding:7px !important; font-size:' . $theme->title_size_product . 'px;">' . stripslashes($row->name) . '</td>';
  if ($params->product_ratings) {
    $content .= '
            <td style="border:none; padding-right:10px;">
              <div style="overflow:hidden; vertical-align:top; height:25px;">
                <div id="voting' . $row->id . '" class="rating_stars" style="width:130px; margin-left:auto;">';
    if ($voted == 0) {
      if ($rating == 0) {
        $title = t('Not rated Yet.');
      }
      else {
        $title = $rating;
      }
      $content .= '
                  <ul class="star-rating">
                    <li class="current-rating" id="current-rating" style="width:' . ($rating * 25) . 'px"></li>
                    <li>
                      <a href="#" onclick=\'spider_catalog_vote(1, ' . $row->id . ', "voting' . $row->id . '", "' . t('Rated.') . '", "' . url('spider_catalog/star_rate', array('query' => array('ex' => 1), 'absolute' => TRUE)) . '"); return false;\'	title="' . $title . '" class="one-star">1</a>
                    </li>
                    <li>
                      <a href="#" onclick=\'spider_catalog_vote(2, ' . $row->id . ', "voting' . $row->id . '", "' . t('Rated.') . '", "' . url('spider_catalog/star_rate', array('query' => array('ex' => 1), 'absolute' => TRUE)) . '"); return false;\'	title="' . $title . '" class="two-stars">2</a>
                    </li>
                    <li>
                      <a href="#" onclick=\'spider_catalog_vote(3, ' . $row->id . ', "voting' . $row->id . '", "' . t('Rated.') . '", "' . url('spider_catalog/star_rate', array('query' => array('ex' => 1), 'absolute' => TRUE)) . '"); return false;\'	title="' . $title . '" class="three-stars">3</a>
                    </li>
                    <li>
                      <a href="#" onclick=\'spider_catalog_vote(4, ' . $row->id . ', "voting' . $row->id . '", "' . t('Rated.') . '", "' . url('spider_catalog/star_rate', array('query' => array('ex' => 1), 'absolute' => TRUE)) . '"); return false;\'	title="' . $title . '" class="four-stars">4</a>
                    </li>
                    <li>
                      <a href="#" onclick=\'spider_catalog_vote(5, ' . $row->id . ', "voting' . $row->id . '", "' . t('Rated.') . '", "' . url('spider_catalog/star_rate', array('query' => array('ex' => 1), 'absolute' => TRUE)) . '"); return false;\'	title="' . $title . '" class="five-stars">5</a>
                    </li>
                  </ul>';
    }
    else {
      if ($rating == 0) {
        $title = t('Not rated Yet.');
      }
      else {
        $title = t('Rating') . ' ' . $rating . '&nbsp;&nbsp;&nbsp;&nbsp;&#013;' . t('You have already rated this product.');
      }
      $content .= '
                <ul class="star-rating1">
                  <li class="current-rating" id="current-rating" style="width:' . ($rating * 25) . 'px"></li>
                  <li><a title="' . $title . '" class="one-star">1</a></li>
                  <li><a title="' . $title . '" class="two-stars">2</a></li>
                  <li><a title="' . $title . '" class="three-stars">3</a></li>
                  <li><a title="' . $title . '" class="four-stars">4</a></li>
                  <li><a title="' . $title . '" class="five-stars">5</a></li>
                </ul>';
    }
    $content .= '
              </div>
            </div>
          </td>';
  }
  $content .= '
        </tr>
      </table>
    </div>
    <table id="prodMiddle" style="border:inherit !impotrtant" cellspacing="0" cellpadding="0">
      <tr>
        <td valign="top" style="width:' . ($theme->picture_width_product) . 'px;">
          <table id="spider_catalog_image_table" cellpadding="0" cellspacing="5" border="0" style="margin:0px;">';
  $imgurl = explode('#***#', $row->image_url);
  if (!($row->image_url != "")) {
    $imgurl[0] = base_path() . drupal_get_path('module', 'spider_catalog') . '/images/no_image.jpg';
    $content .= '
            <tr>
              <td colspan="2" id="prod_main_picture_container" valign="top">
                <div style="width:' . ($theme->picture_width_product) . 'px;height:' . ($theme->picture_height_product) . 'px;border: #CCCCCC solid 2px;padding:5px;background-color:white;">
                  <div id="prod_main_picture" style=\'width:' . ($theme->picture_width_product) . 'px;height:' . ($theme->picture_height_product) . 'px; background:url("' . $imgurl[0] . '") center no-repeat;background-size:contain;height:' . $theme->picture_height_product . ';width:' . $theme->picture_width_product . '\'>&nbsp;</div>
                </div>
              </td>
            </tr>';
  }
  else {
    $content .= '
            <tr>
              <td colspan="2" id="prod_main_picture_container" valign="top">
                <div style="width:' . ($theme->picture_width_product) . 'px;height:' . ($theme->picture_height_product) . 'px;border: #CCCCCC solid 2px;padding:5px;background-color:white;">
                  <a href="' . $imgurl[0] . '" target="_blank" id="prod_main_picture_a" style="text-decoration:none;">
                    <div id="prod_main_picture" style=\'width:' . ($theme->picture_width_product) . 'px;height:' . ($theme->picture_height_product) . 'px; background:url("' . $imgurl[0] . '") center no-repeat;background-size:contain;\'>&nbsp;</div>
                  </a>
                </div>
              </td>
            </tr>';
  }
  $content .= '
            <tr>
              <td style="text-align:justify;">';
  $small_images_str = '';
  $small_images_count = 0;
  $imgurl = explode('#***#', $row->image_url);
  foreach ($imgurl as $img) {
    if ($img !== '') {
      $attach_url = $img;
      $small_images_str .= '
                <a href="' . $img . '" target="_blank">
                  <img style="width:50px;heigth:50px" src="' . $attach_url . '" vspace="0" hspace="0" onMouseOver=\'spider_catalog_prod_change_picture("' . $img . '",this,' . $theme->picture_width_product . ',' . $theme->picture_height_product . ');\' />
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
  $content .= '
              </td>
            </tr>
          </table>
        </td>
        <td valign="top" align="right">';
  if ($params->price && $row->cost != 0 && $row->cost != '') {
    $content .= '
          <div id="prodCost" style="font-size:' . $theme->price_size_product . 'px !important; color:#' . $theme->price_color . ' !important;margin:15px;">' . (($params->currency_symbol_position == 0) ? ($params->currency_symbol) : '') . ' ' . $row->cost . ' ' . (($params->currency_symbol_position == 1) ? $params->currency_symbol : '') . '</div>';
  }
  if ($params->market_price && $row->market_cost != 0 && $row->market_cost != '') {
    $content .= '
          <div id="prodCost" style="font-size:' . ($theme->price_size_product / 1.7) . 'px !important; margin:15px;">' . t('Market Price:') . '
            <span style=" text-decoration:line-through;color:#' . $theme->price_color . ';"> ' . (($params->currency_symbol_position == 0) ? ($params->currency_symbol) : '') . ' ' . $row->market_cost . ' ' . (($params->currency_symbol_position == 1) ? $params->currency_symbol : '') . '</span>
          </div>';
  }
  $content .= '
          <table border="0" id="spider_catalog_informaton_teble" cellspacing="0" cellpadding="5" style="margin:10px;border-width:' . $theme->border_width . 'px !important;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ' !important ;' . (($theme->background_color_review_text != '') ? ('background-color:#' . $theme->background_color_review_text . ';') : '') . '">';
  $param_chan_color = 0;
  if ($category_name != "") {
    $content .= '
            <tr style="' . (($theme->parameters_background_ccolor_1 != '') ? ('background-color:#' . $theme->parameters_background_ccolor_1 . ';') : '') . ' vertical-align:middle;">
              <td>
                <b>' . t('Category:') . '</b>
              </td>
              <td style="' . (($theme->color_of_parameter_values != '') ? ('color:#' . $theme->color_of_parameter_values . ';') : '') . '">
                <span id="cat_' . $row->id . '">' . $category_name . '</span>
              </td>
            </tr>';
    $param_chan_color++;
  }
  else {
    $content .= '
            <span id="cat_' . $row->id . '"></span>';
  }
  $par_data = explode("#@@@#", $row->param);
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
      $bgcolor = (($param_chan_color % 2) ? (($theme->parameters_background_ccolor_2 != '') ? ('background-color:#' . $theme->parameters_background_ccolor_2 . ';') : '') : (($theme->parameters_background_ccolor_1 != '') ? ('background-color:#' . $theme->parameters_background_ccolor_1 . ';') : ''));
      if ($count_of_par != 0) {
        $param_chan_color++;
        $content .= '
            <tr style="' . $bgcolor . 'text-align:left">
              <td><b>' . stripslashes($par1_data[0]) . ':</b></td>
              <td style="' . (($theme->text_size_list != '') ? ('font-size:' . $theme->text_size_list . 'px;') : '') . $bgcolor . (($theme->color_of_parameter_values != '') ? ('color:#' . $theme->color_of_parameter_values . ';') : '') . '">
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
        </td>
      </tr>
    </table>
    <br />
    <div id="prodDescription">' . stripslashes($row->description) . '</div>
    <br /><br />';
  if ($params->customer_reviews) {
    $content .= '
    <div><a name="rev" style="color:inherit;text-decoration:inherit;font-size:150%">' . t('Add your review here') . '</a></div>';
    $url = $link_url . '&rev_page=' . $rev_page;
    $a = 'form';
    $content .= '
    <div style="margin:3px; padding:10px; border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';' . (($theme->background_color_review != '') ? ('background-color:#' . $theme->background_color_review . ';') : '') . '">
      <' . $a . '  action="' . $url . '" name="review" method="post">
        <div style="font-weight:bold;">' . t('Name') . '</div>
        <input type="text" name="full_name" id="full_name" style="width:100%; margin:0px;" />
        <br /><br />
        <div style="font-weight:bold;">' . t('Message:') . '</div>
        <textarea rows="4" name="message_text" id="message_text" style="width:100%; margin:0px;"></textarea>
        <input type="hidden" name="product_id" value="' . $row->id . '" />
        <input type="hidden" name="review" value="1" />
        <br /><br />
        <table cellpadding="0" id="spider_catalog_captcha_table" cellspacing="10" border="0" valign="middle" width="100%">
          <tr>
            <td>' . t('Please enter the code:') . '</td>
            <td style="max-width:100px !important;">
              <span id="wd_captcha">
                <img style="width:80px" src="' . url('spider_catalog/captcha', array('query' => array('ex' => 1), 'absolute' => TRUE)) . '" id="wd_captcha_img" height="24" width="80" />
              </span>
              <a href="javascript:spider_catalog_refresh_captcha();" style="text-decoration:none">&nbsp;
                <img src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/refresh.png" border="0" style="border:none" />
              </a>&nbsp;
            </td>
            <td><input type="text" name="code" id="review_capcode" size="6" />
              <span id="caphid"></span>
            </td>
            <td style="text-align:right !important;" align="right">
              <input type="button" class="spidercatalogbutton" style="background:#' . $theme->buttons_background_color . '; color:#' . $theme->color_of_text_of_buttons . '; width:inherit;margin-right:10px;" value="' . t('Send') . '" onclick=\'spider_catalog_submit_reveiw("' . t('The Name field is required.') . '","' . t('The Message field is required.') . '","' . t('Sorry, the code you entered was invalid.') . '");\' />
            </td>
          </tr>
        </table>
      </form>
    </div>';
    if (isset($_POST['code'])) {
      $code = $_POST['code'];
    }
    else {
      $code = '';
    }
    if (isset($_POST['review'])) {
      $review = $_POST['review'];
    }
    else {
      $review = '';
    }
    if ($review) {
      if ($code != '' && $code == $_SESSION['spider_catalog_captcha_code']) {
        drupal_set_message(t('The review has been added successfully.'), 'status', FALSE);
        $content .= '
    <br />
    <center style="font-weight:bold">' . t('The review has been added successfully.') . '</center>
    <br />';
        drupal_goto($link_url);
      }
      else {
        $content .= '
    <br />
    <center style="font-weight:bold">' . t('Sorry, the code you entered was invalid.') . '</center>
    <br />';
      }
    }
    $reviews_count = db_select('spider_catalog_product_reviews')
      ->condition('product_id', $product_id, '=')
      ->countQuery()
      ->execute()
      ->fetchField();
    foreach ($reviews_rows as $reviews_row_id) {
      $reviews_row = db_query("SELECT * FROM {spider_catalog_product_reviews} WHERE id=:id", array(':id' => $reviews_row_id))->fetchObject();
      $content .= '
    <br /><br />
    <div style="padding:3px;' . (($theme->background_color_review_author != '') ? ('background-color:#' . $theme->background_color_review_author . ';') : '') . '">' . t('Author:') . ' <b>' . $reviews_row->name . '</b></div>
    <div style="' . (($theme->background_color_review_text != '') ? ('background-color:#' . $theme->background_color_review_text . ';') : '') . (($theme->color_of_review_texts != '') ? ('color:#' . $theme->color_of_review_texts . ';') : '') . ' padding:8px;">' . str_replace('
', '<br />', stripslashes($reviews_row->content)) . '
    </div>';
    }
    if ($reviews_count > $reviews_perpage) {
      $r = ceil($reviews_count / $reviews_perpage);
      $navstyle = (($theme->text_size != '') ? ('font-size:' . $theme->text_size . 'px !important;') : 'font-size:12px !important;') . (($theme->text_color != '') ? ('color:#' . $theme->text_color . ' !important;') : '');
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
      if ($rev_page > 5) {
        $link = $link_url . '&rev_page=1#rev';
        $content .= '
    &nbsp;&nbsp;
    <a href=\'javascript:spider_catalog_submit_catal("' . $link . '")\' style="' . $navstyle . '">' . t('first') . '</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;...&nbsp;';
      }
      if ($rev_page > 1) {
        $link = $link_url . '&rev_page=' . ($rev_page - 1) . '#rev';
        $content .= '
    &nbsp;&nbsp;
    <a href=\'javascript:spider_catalog_submit_catal("' . $link . '")\' style="' . $navstyle . '">' . t('prev') . '</a>&nbsp;&nbsp;';
      }
      for ($i = $rev_page - 4; $i < ($rev_page + 5); $i++) {
        if ($i <= $r && $i >= 1) {
          $link = $link_url . '&rev_page=' . $i . '#rev';
          if ($i == $rev_page) {
            $content .= '
    <span style="font-weight:bold !important; color:#000000 !important; ' . (($theme->text_size != '') ? ('font-size:' . $theme->text_size . 'px !important;') : 'font-size:12px !important;') . '">&nbsp;' . $i . '&nbsp;</span>';
          }
          else {
            $content .= '
    <a href=\'javascript:spider_catalog_submit_catal("' . $link . '")\' style="' . $navstyle . '">&nbsp;' . $i . '&nbsp;</a>';
          }
        }
      }
      if ($rev_page < $r) {
        $link = $link_url . '&rev_page=' . ($rev_page + 1) . '#rev';
        $content .= '
    &nbsp;&nbsp;
    <a href=\'javascript:spider_catalog_submit_catal("' . $link . '")\' style="' . $navstyle . '">' . t('next') . '</a>&nbsp;&nbsp;';
      }
      if (($r - $rev_page) > 4) {
        $link = $link_url . '&rev_page=' . $r . '#rev';
        $content .= '
    &nbsp;...&nbsp;&nbsp;&nbsp;
    <a href=\'javascript:spider_catalog_submit_catal("' . $link . '")\' style="' . $navstyle . '">' . t('last') . '</a>';
      }
      $content .= '
    </div>';
    }
  }
  $content .= '
  </div>';
  $content .= '
  </div>
  <br /><br />';
  drupal_add_js('var SpiderCatOFOnLoad = window.onload;window.onload = SpiderCatAddToOnload;', array('type' => 'inline', 'scope' => 'footer'));
  return $content;
}
