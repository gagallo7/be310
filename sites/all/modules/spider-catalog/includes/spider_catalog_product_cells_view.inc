<?php
/**
 * @file
 * This file contains all functions which need for calendar view.
 */

/**
 * View products in cells.
 */
function spider_catalog_product_cells($theme_id, $category_id, $show_category_details) {
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
    ), 'setting');
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
  $prod_in_page = $theme->count_of_products_in_the_row * $theme->count_of_rows_in_the_page;
  $rows = db_query("SELECT id FROM {spider_catalog_products} WHERE published=:published AND category_id" . $compare . ":category_id AND name LIKE :name ORDER BY ordering LIMIT " . (($page_num - 1) * $prod_in_page) . ", " . $prod_in_page, array(
    ':published' => 1,
    ':category_id' => $cat_id,
    ':name' => '%' . db_like($prod_name) . '%'))->fetchCol();
  $prod_count = db_query("SELECT COUNT(id) FROM {spider_catalog_products} WHERE category_id" . $compare . ":category_id AND name LIKE :name", array(':category_id' => $cat_id, ':name' => '%' . db_like($prod_name) . '%'))->fetchField();
  $permalink_for_sp_cat = url('spider_catalog/product_view', array(
    'query' => array('theme_id' => $theme_id),
    'absolute' => TRUE,
  ));
  $page_link = url('node/' . $nodeid, array('query' => array('example' => 1), 'absolute' => TRUE));
  $content = '';
  $prod_iterator = 0;
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
      #productMainDiv,
      .spidercatalogbutton,
      .spidercataloginput {
        -moz-border-radius: ' . $border_radius . ' !important;
        -webkit-border-radius: ' . $border_radius . ' !important;
        border-radius: ' . $border_radius . ' !important;
        font-size:12px !important;
      }
      #productMainDiv,
      .spidercatalogbutton,
      .spidercataloginput {
        -moz-border-radius: ' . $border_radius . ';
        -webkit-border-radius: ' . $border_radius . ';
        border-radius: ' . $border_radius . ';
      }
      #productMainDiv #prodTitle {
        -moz-border-radius-topleft: ' . $border_radius . ';
        -moz-border-radius-topright: ' . $border_radius . ';
        -webkit-border-top-left-radius: ' . $border_radius . ';
        -webkit-border-top-right-radius: ' . $border_radius . ';
        border-top-left-radius: ' . $border_radius . ';
        border-top-right-radius: ' . $border_radius . ';
      }
      #productMainDiv,
      .spidercatalogbutton {
        -moz-border-radius: ' . $border_radius . ' !important;
        -webkit-border-radius: ' . $border_radius . ' !important;
        border-radius: ' . $border_radius . ' !important;
        font-size:10px !important;
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
        -moz-box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.075);
        -moz-box-sizing: border-box;
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
        font-style: normal;
        font-family: Arial, "Liberation Sans", FreeSans, sans-serif;
        font-weight: normal;
      }
      #productMainDiv optgroup::-moz-focus-inner {
        border: 0;
        padding: 0;
      }
      #productMainTable{
        border: none !important;
        border-collapse: inherit;
        margin-bottom: 0px;
        margin-left: 0px;
        margin-right: 0px;
        margin-top: 0px;
        padding-bottom: 0px;
        padding-left: 0px;
        padding-right: 0px;
        padding-top: 0px;
      }
      #productMainTable table,
      #productMainTable tbody,
      #productMainTable tr,
      #productMainTable td {
        background-color: inherit;
        border: none !important;
        border-collapse: inherit;
        margin-bottom: 0px!important;
        margin-left: 0px!important;
        margin-right: 0px!important;
        margin-top: 0px !important;
        opacity: 1 !important;
        padding: inherit;
        padding-bottom: 0px!important;
        padding-left: 0px!important;
        padding-right: 0px!important;
        padding-top: 0px!important;
        text-align: left;
      }
      .CatalogSearchBox input {
        margin-top: -3px !important;
      }
      .S_P_productMainDiv table,
      .S_P_productMainDiv tr,
      .S_P_productMainDiv td,
      .S_P_productMainDiv tbody,
      .S_P_productMainDiv table tr td {
          border: none !important;
      }
      #productMainDiv ul li,
      #productMainDiv ul,
      #productMainDiv li {
        list-style-type: none !important;
      }
      #productMainDiv td,
      #productMainDiv tr,
      #productMainDiv tbody,
      #productMainDiv div {
        background-color:inherit;
        color: inherit;
        line-height: inherit !important;
        max-height: inherit !important;
        max-width: inherit !important;
        opacity: inherit !important;
      }
      #productMainDiv,
      #productMainDiv div {
        max-width: 1000000px !important;
      }
      #prodMiddle,
      #prodMiddle a,
      #prodMiddle li,
      #prodMiddle ol{
        background-color: inherit !important;
      }
      #tdviewportheight div,
      #tdviewportheight td,
      #tdviewportheight {
        vertical-align: middle;
      }
    </style>';
  // $params1['categories'] > 0
  if ((($cat_id > 0) || ($cat_id != 0)) && ($show_category_details == 1)) {
    if ($params->category_details_width != '') {
      $category_details_width = 'width:' . $params->category_details_width . 'px !important;';
    }
    else {
      $category_details_width = '';
    }
    $imgurl = explode('#***#', $cat_rows->category_image_url);
    $content .= '
      <div id="productMainDiv" class="S_P_productMainDiv" style="' . $category_details_width . 'border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';' . (($theme->text_size != '') ? ('font-size:' . $theme->text_size . 'px !important;') : '') . (($theme->text_color != '') ? ('color:#' . $theme->text_color . ';') : '') . (($theme->background_color != '') ? ('background-color:#' . $theme->background_color . ';') : '') . '">
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
  $a = 'form';
  $content .= '
      <div id="productMainDiv" style="text-align:center; width:' . ($theme->count_of_products_in_the_row * $theme->product_cell_width + $theme->count_of_products_in_the_row * 35) . 'px">';
  if ($params->search_by_category || $params->search_by_name) {
    $content .= '
      <' . $a . ' action="' . $page_link . '" method="post" name="cat_form" id="cat_form_page_nav">
        <input type="hidden" name="page_num"	value="1">
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
      <table cellpadding="0" cellspacing="0" id="productMainTable" style="width:' . ($theme->count_of_products_in_the_row * $theme->product_cell_width + $theme->count_of_products_in_the_row * 20) . 'px">
        <tr>';
  }
  foreach ($rows as $row) {
    $row_param = db_query("SELECT * FROM {spider_catalog_products} WHERE id=:id", array(':id' => $row))->fetchObject();
    if ((($prod_iterator % $theme->count_of_products_in_the_row) === 0) && ($prod_iterator > 0)) {
      $content .= '
        </tr>
        <tr>';
    }
    $prod_iterator++;
    $link = $permalink_for_sp_cat . '&product_id=' . $row_param->id . '&page_num=' . $page_num . '&back=' . $nodeid;
    $imgurl = explode('#***#', $row_param->image_url);
    $content .= '
          <td>
            <div id="productMainDiv" style="border-width:' . $theme->border_width . 'px;border-color:#' . $theme->border_color . ';border-style:' . $theme->border_style . ';' . (($theme->text_size != '') ? ('font-size:' . $theme->text_size . 'px !important;') : '') . (($theme->text_color != '') ? ('color:#' . $theme->text_color . ';') : '') . (($theme->background_color != '') ? ('background-color:#' . $theme->background_color . ';') : '') . ' width:' . $theme->product_cell_width . 'px; height:' . $theme->product_cell_height . 'px;">
              <div style="height:' . ($theme->product_cell_height - 20) . 'px;">
                <div id="prodTitle" style="font-size:' . $theme->title_size . 'px;' . (($theme->title_color != '') ? ('color:#' . $theme->title_color . ';') : '') . (($theme->title_background_color != '') ? ('background-color:#' . $theme->title_background_color . ';') : '') . '">
                  <a href="' . $link . '" style="' . (($theme->title_color != '') ? ('color:#' . $theme->title_color . ';') : '') . '">' . stripslashes($row_param->name) . '</a>
                </div>
                <table id="prodMiddle" border="0" cellspacing="0" cellpadding="0">
                  <tr>';
    if (!($row_param->image_url != '')) {
      $content .= '
                    <td style="padding:10px;">
                      <img style="max-width:' . $theme->picture_width . 'px; max-height:' . $theme->picture_height . 'px" src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/no_image.jpg" />
                    </td>';
    }
    else {
      $content .= '
                    <td style="padding:10px;">
                      <a href="' . $imgurl[0] . '" target="_blank">
                        <img style="max-width:' . $theme->picture_width . 'px; max-height:' . $theme->picture_height . 'px" src="' . $imgurl[0] . '" />
                      </a>
                    </td>';
    }
    $content .= '
                    <td>';
    if ($params->price && ($row_param->cost != 0) && ($row_param->cost != '')) {
      $content .= '
                      <div id="prodCost" style="font-size:' . $theme->price_size . 'px !important;color:#' . $theme->price_color . ';">' . (($params->currency_symbol_position == 0) ? ($params->currency_symbol) : '') . ' ' . $row_param->cost . ' ' . (($params->currency_symbol_position == 1) ? $params->currency_symbol : '') . '</div>';
    }
    if ($params->market_price && ($row_param->market_cost != 0) && ($row_param->market_cost != '')) {
      $content .= '
                      <div id="prodCost" style="font-size:' . ($theme->price_size / 1.7) . 'px !important;">' . t('Market Price:') . ' 
                        <span style=" text-decoration:line-through;color:#' . $theme->price_color . ';"> ' . (($params->currency_symbol_position == 0) ? ($params->currency_symbol) : '') . ' ' . $row_param->market_cost . ' ' . (($params->currency_symbol_position == 1) ? $params->currency_symbol : '') . '</span>
                      </div>';
    }
    $content .= '
                    </td>
                  </tr>
                  <tr>
                    <td>';
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
                    </td>
                    <td></td>
                  </tr>
                  <tr>
                    <td colspan="2">
                      <table border="0" cellspacing="0" cellpadding="0" width="100%">';
    if ($theme->show_category) {
      if ($row_param->category_id == 0) {
        $category_title = t('Uncategorized');
      }
      else {
        $category_title = $category_list[$row_param->category_id];
      }
      $content .= '
                        <tr style="' . (($theme->parameters_background_ccolor_1 != '') ? ('background-color:#' . $theme->parameters_background_ccolor_1 . ';') : '') . ' vertical-align:middle;">
                          <td><b>' . t('Category:') . '</b></td>
                          <td style="' . (($theme->color_of_parameter_values != '') ? ('color:#' . $theme->color_of_parameter_values . ';') : '') . '">
                            <span id="cat_' . $row_param->id . '">' . $category_title . '</span>
                          </td>
                        </tr>';
    }
    else {
      $content .= '
                        <span id="cat_' . $row_param->id . '"></span>';
    }
    $par_data = explode("#@@@#", $row_param->param);
    if ($theme->show_parameters) {
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
                          <td style="' . (($theme->text_size != '') ? ('font-size:' . $theme->text_size . 'px;') : '') . $bgcolor . (($theme->color_of_parameter_values != '') ? ('color:#' . $theme->color_of_parameter_values . ';') : '') . /*'width:' . $params['parameters_select_box_width'] . 'px;*/'">
                            <ul class="spidercatalogparamslist">';
            for ($k = 0; $k < count($par_values); $k++) {
              if ($par_values[$k] != "") {
                $content .= ' <li>' . stripslashes($par_values[$k]) . '</li>';
              }
            }
            $content .= '   </ul>
                          </td>
                        </tr>';
          }
        }
      }
    }
    $description = $row_param->short_description;
    $content .= '     </table>
                      <div id="prodDescription">' . $description . '</div>
                    </td>
                  </tr>
                </table>
              </div>
              <div id="prodMore">
                <a href="' . $link . '" style="' . (($theme->hyperlink_color != '') ? ('color:#' . $theme->hyperlink_color . ';') : '') . '">' . t('More') . '</a>
              </div>
            </div>
          </td>';
  }
  if (count($rows)) {
    $content .= '
        <tr>
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
  if ($cat_id != 0) {
    $page_link .= "&cat_id=" . $cat_id;
  }
  if ($prod_name != "") {
    $page_link .= "&prod_name=" . $prod_name;
  }
  if (($prod_count > $prod_in_page) && ($prod_in_page > 0)) {
    $r = ceil($prod_count / $prod_in_page);
    $navstyle = (($theme->text_size != '') ? ('font-size:' . $theme->text_size . 'px !important;') : 'font-size:12px !important;') . (($theme->text_color != '') ? ('color:#' . $theme->text_color . ' !important;') : '');
    $link = $page_link . '&page_num= ';
    if ($page_num > 5) {
      $link = $page_link . '&page_num=1';
      $content .= '
        &nbsp;&nbsp;
        <a href=\'javascript:spider_catalog_submit_catal("' . $link . '")\' style="' . $navstyle . '">' . t('First') . '</a>&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;...&nbsp;';
    }
    if ($page_num > 1) {
      $link = $page_link . '&page_num=' . ($page_num - 1);
      $content .= '
        &nbsp;&nbsp;
        <a href=\'javascript:spider_catalog_submit_catal("' . $link . '")\' style="' . $navstyle . '">' . t('Prev') . '</a>&nbsp;&nbsp;';
    }
    for ($i = $page_num - 4; $i < ($page_num + 5); $i++) {
      if ($i <= $r and $i >= 1) {
        $link = $page_link . '&page_num=' . $i;
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
      $link = $page_link . '&page_num=' . ($page_num + 1);
      $content .= '
        &nbsp;&nbsp;
        <a href=\'javascript:spider_catalog_submit_catal("' . $link . '")\' style="' . $navstyle . '">' . t('Next') . '</a>&nbsp;&nbsp;';
    }
    if (($r - $page_num) > 4) {
      $link = $page_link . '&page_num=' . $r;
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
