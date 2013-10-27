<?php
/**
 * @file
 * Spider Catalog module catalog parameters.
 */

/**
 * Menu loader callback. Load catalog parameters.
 */
function spider_catalog_products() {
  $form = array();
  $free_version = '<a href="http://web-dorado.com/products/drupal-catalog.html" target="_blank" style="color:red; text-decoration:none; float:right;">
                    <img src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/header.png" border="0" alt="www.web-dorado.com" width="215"><br />
                  <div style="float:right;">' . t('Get the full version') . '&nbsp;&nbsp;&nbsp;&nbsp;</div>
                  </a>';
  $form['fieldset_product_buttons'] = array(
    '#prefix' => $free_version,
    '#type' => 'fieldset',
  );
  $form['fieldset_product_buttons']['publish_product'] = array(
    '#type' => 'submit',
    '#submit' => array('spider_catalog_products_publish'),
    '#value' => t('Publish'),
  );
  $form['fieldset_product_buttons']['unpublish_product'] = array(
    '#type' => 'submit',
    '#submit' => array('spider_catalog_products_unpublish'),
    '#value' => t('Unpublish'),
  );
  $form['fieldset_product_buttons']['delete_product'] = array(
    '#type' => 'submit',
    '#submit' => array('spider_catalog_products_delete'),
    '#value' => t('Delete'),
    '#attributes' => array('onclick' => 'if (!confirm(Drupal.t("Do you want to delete selected products?"))) {return false;}'),
  );
  $form['fieldset_product_buttons']['save_order'] = array(
    '#type' => 'submit',
    '#submit' => array('spider_catalog_product_save_ordering'),
    '#value' => t('Save order'),
  );
  $form['fieldset_product_buttons']['new_product'] = array(
    '#prefix' => l(t('New'), url('admin/settings/spider_catalog/products/edit', array('absolute' => TRUE))),
  );
  $form['fieldset_search_products_name'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search products by name'),
    '#collapsible' => TRUE,
    '#collapsed' => ((variable_get('spider_catalog_search_products_name', '') == '') ? TRUE : FALSE),
  );
  $form['fieldset_search_products_name']['search_products_name'] = array(
    '#type' => 'textfield',
    '#size' => 25,
    '#default_value' => variable_get('spider_catalog_search_products_name', ''),
  );
  $form['fieldset_search_products_name']['search_products'] = array(
    '#type' => 'submit',
    '#submit' => array('spider_catalog_search_products_name'),
    '#value' => t('Go'),
  );
  $form['fieldset_search_products_name']['reset_products'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('spider_catalog_reset_products_name'),
  );

  drupal_add_tabledrag('products_table-components', 'order', 'sibling', 'weight');
  $header = array(
    'checkbox' => array('class' => 'select-all'),
    'id' => array('data' => t('ID'), 'field' => 'n.id'),
    'name' => array('data' => t('Name'), 'field' => 'n.name'),
    'description' => array('data' => t('Description'), 'field' => 'n.description'),
    'order' => array(
      'data' => t('Order'),
      'field' => 'n.ordering',
      'sort' => 'asc'),
    'category' => array('data' => t('Category'), 'field' => 'n.category_id'),
    'published' => array('data' => t('Published')),
    'delete' => array('data' => t('Delete')),
  );
  $options = array();
  $products_ids = db_select('spider_catalog_products', 'n')
    ->fields('n', array('id'))
    ->condition('n.name', '%' . db_like(variable_get('spider_catalog_search_products_name', '')) . '%', 'LIKE')
    ->extend('TableSort')
    ->orderByHeader($header)
    ->extend('PagerDefault')
    ->limit(20)
    ->execute()
    ->fetchCol();
  foreach ($products_ids as $product_id) {
    $row = db_query("SELECT * FROM {spider_catalog_products} WHERE id=:id", array(':id' => $product_id))->fetchObject();
    if ($row->published) {
      $publish_unpublish_png = 'publish.png';
      $publish_unpublish_function = 'unpublish';
    }
    else {
      $publish_unpublish_png = 'unpublish.png';
      $publish_unpublish_function = 'publish';
    }
    $category_name = db_query("SELECT name FROM {spider_catalog_product_categories} WHERE id=:id", array(':id' => $row->category_id))->fetchField();
    if ($row->category_id == 0) {
      $category_name = t('Uncategorised');
    }
    $options[$product_id] = array(
      'checkbox' => array(
        'data' => array(
          '#type' => 'checkbox',
          '#attributes' => array('name' => 'spider_catalog_product_check_' . $product_id),
        ),
      ),
      'id' => $product_id,
      'name' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => $row->name,
          '#href' => url('admin/settings/spider_catalog/products/edit', array('query' => array('product_id' => $product_id), 'absolute' => TRUE)),
        ),
      ),
      'description' => $row->description,
      'order' => array(
        'data' => array(
          '#type' => 'textfield',
          '#size' => 3,
          '#value' => $row->ordering,
          '#attributes' => array('name' => 'spider_catalog_products_order_' . $product_id, 'class' => array('weight')),
        ),
      ),
      'category' => $category_name,
      'published' => l('<img src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/' . $publish_unpublish_png . '" />', url('admin/settings/spider_catalog/products/' . $publish_unpublish_function, array('query' => array('product_id' => $product_id), 'absolute' => TRUE)), array('html' => TRUE)),
      'delete' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => t('Delete'),
          '#href' => url('admin/settings/spider_catalog/products/delete', array('query' => array('product_id' => $product_id), 'absolute' => TRUE)),
        ),
      ),
    );
    $options[$product_id]['#attributes'] = array('class' => array('draggable'));
  }
  $form['products_table'] = array(
    '#type' => 'tableselect',
    '#js_select' => TRUE,
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No products available.'),
    '#suffix' => theme('pager', array('tags' => array())),
    '#attributes' => array('id' => 'products_table-components'),
  );

  foreach ($products_ids as $product_id) {
    $form['products_table'][$product_id]['#disabled'] = TRUE;
  }
  return $form;
}

/**
 * Search in products.
 */
function spider_catalog_search_products_name($form, &$form_state) {
  if ($form_state['values']['search_products_name'] != '') {
    variable_set('spider_catalog_search_products_name', $form_state['values']['search_products_name']);
  }
  else {
    variable_set('spider_catalog_search_products_name', '');
  }
  $form_state['redirect'] = url('admin/settings/spider_catalog/products', array('absolute' => TRUE));
}

/**
 * Reset products.
 */
function spider_catalog_reset_products_name($form, &$form_state) {
  variable_set('spider_catalog_search_products_name', '');
  $form_state['redirect'] = url('admin/settings/spider_catalog/products', array('absolute' => TRUE));
}

/**
 * Publish selected products.
 */
function spider_catalog_products_publish($form, &$form_state) {
  if (db_query("SELECT id FROM {spider_catalog_products}")) {
    $product_ids_col = db_query("SELECT id FROM {spider_catalog_products}")->fetchCol();
    $flag = FALSE;
    foreach ($product_ids_col as $product_id) {
      if (isset($_POST['spider_catalog_product_check_' . $product_id])) {
        $flag = TRUE;
        db_query("UPDATE {spider_catalog_products} SET published=:published WHERE id=:id", array(':published' => 1, ':id' => $product_id));
      }
    }
    if ($flag == FALSE) {
      drupal_set_message(t('You must select at least one product.'), 'warning', FALSE);
    }
    else {
      drupal_set_message(t('Products successfully published.'), 'status', FALSE);
    }
  }
}

/**
 * Publish product.
 */
function spider_catalog_product_publish() {
  if (isset($_GET['product_id'])) {
    $product_id = check_plain($_GET['product_id']);
    db_query("UPDATE {spider_catalog_products} SET published=:published WHERE id=:id", array(':published' => 1, ':id' => $product_id));
  }
  drupal_set_message(t('Product successfully published.'), 'status', FALSE);
  drupal_goto(url('admin/settings/spider_catalog/products', array('absolute' => TRUE)));
}

/**
 * Unpublish selected products.
 */
function spider_catalog_products_unpublish($form, &$form_state) {
  if (db_query("SELECT id FROM {spider_catalog_products}")) {
    $product_ids_col = db_query("SELECT id FROM {spider_catalog_products}")->fetchCol();
    $flag = FALSE;
    foreach ($product_ids_col as $product_id) {
      if (isset($_POST['spider_catalog_product_check_' . $product_id])) {
        $flag = TRUE;
        db_query("UPDATE {spider_catalog_products} SET published=:published WHERE id=:id", array(':published' => 0, ':id' => $product_id));
      }
    }
    if ($flag == FALSE) {
      drupal_set_message(t('You must select at least one product.'), 'warning', FALSE);
    }
    else {
      drupal_set_message(t('Products successfully unpublished.'), 'status', FALSE);
    }
  }
}

/**
 * Unpublish product.
 */
function spider_catalog_product_unpublish() {
  if (isset($_GET['product_id'])) {
    $product_id = check_plain($_GET['product_id']);
    db_query("UPDATE {spider_catalog_products} SET published=:published WHERE id=:id", array(':published' => 0, ':id' => $product_id));
  }
  drupal_set_message(t('Product successfully unpublished.'), 'status', FALSE);
  drupal_goto(url('admin/settings/spider_catalog/products', array('absolute' => TRUE)));
}

/**
 * Delete selected products.
 */
function spider_catalog_products_delete($form, &$form_state) {
  if (db_query("SELECT id FROM {spider_catalog_products}")) {
    $product_ids_col = db_query("SELECT id FROM {spider_catalog_products}")->fetchCol();
    $flag = FALSE;
    foreach ($product_ids_col as $product_id) {
      if (isset($_POST['spider_catalog_product_check_' . $product_id])) {
        $flag = TRUE;
        db_query("DELETE FROM {spider_catalog_products} WHERE id=:id", array(':id' => $product_id));
        db_query("DELETE FROM {spider_catalog_product_votes} WHERE product_id=:product_id", array(':product_id' => $product_id));
        db_query("DELETE FROM {spider_catalog_product_reviews} WHERE product_id=:product_id", array(':product_id' => $product_id));
        drupal_set_message(t('Selected products successfully deleted.'), 'status', FALSE);
      }
    }
    if ($flag == FALSE) {
      drupal_set_message(t('You must select at least one product.'), 'warning', FALSE);
    }
    else {
      drupal_set_message(t('Products successfully deleted.'), 'status', FALSE);
    }
  }
}

/**
 * Delete product.
 */
function spider_catalog_product_delete() {
  if (isset($_GET['product_id'])) {
    $product_id = check_plain($_GET['product_id']);
    db_query("DELETE FROM {spider_catalog_products} WHERE id=:id", array(':id' => $product_id));
    db_query("DELETE FROM {spider_catalog_product_votes} WHERE product_id=:product_id", array(':product_id' => $product_id));
    db_query("DELETE FROM {spider_catalog_product_reviews} WHERE product_id=:product_id", array(':product_id' => $product_id));
  }
  drupal_set_message(t('Product successfully deleted.'), 'status', FALSE);
  drupal_goto(url('admin/settings/spider_catalog/products', array('absolute' => TRUE)));
}

/**
 * Save ordering.
 */
function spider_catalog_product_save_ordering($form, &$form_state) {
  $products_ids = db_query("SELECT id FROM {spider_catalog_products}")->fetchCol();
  foreach ($products_ids as $product_id) {
    if (isset($_POST['spider_catalog_products_order_' . $product_id])) {
      $ordering = check_plain($_POST['spider_catalog_products_order_' . $product_id]);
      db_query("UPDATE {spider_catalog_products} SET ordering=:ordering WHERE id=:id", array(':ordering' => $ordering, ':id' => $product_id));
    }
    drupal_set_message(t('Products ordering successfully saved.'), 'status', FALSE);
  }
}

/**
 * Add or edit product.
 */
function spider_catalog_product_edit() {
  drupal_add_js(drupal_get_path('module', 'spider_catalog') . '/js/spider_catalog_attach_file.js');
  drupal_add_js(drupal_get_path('module', 'spider_catalog') . '/js/spider_catalog_params.js');
  if (isset($_GET['product_id']) && ($_GET['product_id'] != '')) {
    $product_id = check_plain($_GET['product_id']);
    $row = db_query("SELECT * FROM {spider_catalog_products} WHERE id=:id", array(':id' => $product_id))->fetchObject();
    $product_name = $row->name;
    $product_price = $row->cost;
    $product_market_price = $row->market_cost;
    $product_images = $row->image_url;
    $images_array = explode('#***#', $product_images);
    $product_description = $row->description;
    $product_short_description = $row->short_description;
    $product_published = $row->published;
    if (isset($_GET['category_id'])) {
      $product_category_id = check_plain($_GET['category_id']);
      $category_param = db_query("SELECT param FROM {spider_catalog_product_categories} WHERE id=:id", array(':id' => $product_category_id))->fetchField();
      $product_param = str_replace('#***#', '#@@@#', $category_param);
    }
    else {
      $product_category_id = $row->category_id;
      $product_param = $row->param;
    }
  }
  else {
    if ((isset($_GET['category_id'])) && ($_GET['category_id'] != 0)) {
      $product_category_id = check_plain($_GET['category_id']);
      $category_param = db_query("SELECT param FROM {spider_catalog_product_categories} WHERE id=:id", array(':id' => $product_category_id))->fetchField();
      $product_param = str_replace('#***#', '#@@@#', $category_param);
      $product_name = '';
      $product_price = '';
      $product_market_price = '';
      $product_description = '';
      $product_short_description = '';
    }
    else {
      $product_category_id = 0;
      $product_param = '';
      $product_name = '';
      $product_price = '';
      $product_market_price = '';
      $product_description = '';
      $product_short_description = '';
    }
    $product_id = '';
    $product_images = '';
    $images_array = '';
    $product_published = 1;
  }
  drupal_add_js(array(
    'spider_catalog' => array(
      'delete_png_url' => base_path() . drupal_get_path('module', 'spider_catalog') . '/images/',
      'select_action' => url('admin/settings/spider_catalog/products/edit', array('query' => array('product_id' => $product_id), 'absolute' => TRUE)),
    ),
    ),
    'setting');
  $categories_id_array[0] = t('Uncategorised');
  $categories_id_array += db_query("SELECT id,name FROM {spider_catalog_product_categories} ORDER BY name")->fetchAllKeyed();
  if (file_exists("sites/all/libraries/tinymce/jscripts/tiny_mce/tiny_mce.js")) {
    drupal_add_js('sites/all/libraries/tinymce/jscripts/tiny_mce/tiny_mce.js');
    drupal_add_js('tinyMCE.init({
				// General options
        mode : "specific_textareas",
        editor_selector : "spider_catalog_editor",
				theme : "advanced",
				plugins : "autolink,lists,spellchecker,pagebreak,style,layer,table,save,advhr,advimage,advlink,emotions,iespell,inlinepopups,insertdatetime,preview,media,searchreplace,print,contextmenu,paste,directionality,fullscreen,noneditable,visualchars,nonbreaking,xhtmlxtras,template",
				// Theme options
				theme_advanced_buttons1 : "save,newdocument,|,bold,italic,underline,strikethrough,|,justifyleft,justifycenter,justifyright,justifyfull,|,styleselect,formatselect,fontselect,fontsizeselect",
				theme_advanced_buttons2 : "cut,copy,paste,pastetext,pasteword,|,search,replace,|,bullist,numlist,|,outdent,indent,blockquote,|,undo,redo,|,link,unlink,anchor,image,cleanup,help,code,|,insertdate,inserttime,preview,|,forecolor,backcolor",
				theme_advanced_buttons3 : "tablecontrols,|,hr,removeformat,visualaid,|,sub,sup,|,charmap,emotions,iespell,media,advhr,|,print,|,ltr,rtl,|,fullscreen",
				theme_advanced_buttons4 : "insertlayer,moveforward,movebackward,absolute,|,styleprops,spellchecker,|,cite,abbr,acronym,del,ins,attribs,|,visualchars,nonbreaking,template,blockquote,pagebreak,|,insertfile,insertimage",
				theme_advanced_toolbar_location : "top",
				theme_advanced_toolbar_align : "left",
				theme_advanced_statusbar_location : "bottom",
				theme_advanced_resizing : true,

				// Skin options
				skin : "o2k7",
				skin_variant : "silver",

				// Example content CSS (should be your site CSS)
				//content_css : "css/example.css",
				
				// Drop lists for link/image/media/template dialogs
				template_external_list_url : "js/template_list.js",
				external_link_list_url : "js/link_list.js",
				external_image_list_url : "js/image_list.js",
				media_external_list_url : "js/media_list.js",

				// Replace values for the template plugin
				template_replace_values : {
					username : "Some User",
					staffid : "991234"
				}
			});', array('type' => 'inline', 'scope' => 'footer'));
  }
  $form = array();
  $form['product_edit_ratings'] = array(
    '#type' => 'link',
    '#href' => url('admin/settings/spider_catalog/products/edit/ratings', array('query' => array('product_id' => $product_id), 'absolute' => TRUE)),
    '#title' => t('Edit ratings'),
    '#attributes' => array('class' => array('button')),
  );
  $form['product_edit_reviews'] = array(
    '#type' => 'link',
    '#href' => url('admin/settings/spider_catalog/products/edit/reviews', array('query' => array('product_id' => $product_id), 'absolute' => TRUE)),
    '#title' => t('Edit reviews'),
    '#attributes' => array('class' => array('button')),
  );
  drupal_add_js('
    var product_images_array = "' . $product_images . '";
    var parameters0 = new Array();
    parameters0["sel2"] = product_images_array.split("#***#");', array('type' => 'inline'));
  $form['product_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
    '#default_value' => $product_name,
    '#size' => 40,
  );
  $form['product_categories'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#default_value' => $product_category_id,
    '#options' => $categories_id_array,
    '#attributes' => array('onchange' => 'spider_catalog_change_category();'),
  );
  $form['product_price'] = array(
    '#type' => 'textfield',
    '#title' => t('Price'),
    '#default_value' => $product_price,
    '#size' => 15,
  );
  $form['product_market_price'] = array(
    '#type' => 'textfield',
    '#title' => t('Market Price'),
    '#default_value' => $product_market_price,
    '#size' => 15,
  );
  $par_image = explode("#***#", $product_images);
  $k = 0;
  $image_items = '<strong>' . t('Images') . '</strong><div id="sel2">';
  $show_images = '<div id="images_div" style="width:470px;">';
  while ($k < 1000) {
    if (isset($par_image[$k]) && $par_image[$k] != '') {
      $image_items .= '
        <input type="text" class="form-text" style="width:200px;" id="inp_sel2_' . $k . '" value="' . $par_image[$k] . '" onChange=\'spider_catalog_add("sel2");\' />
        <img src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/delete.png" style="cursor:pointer;" onclick=\'spider_catalog_remove(' . $k . ', "sel2");spider_catalog_delete_view_image(' . $k . ');\'><br />';
      $show_images .= '
        <a id="view_image_' . $k . '" title="' . $par_image[$k] . '" target="_blank" href="' . $par_image[$k] . '">
          <img style="max-height:50px; max-width:50px; margin:8px 0 8px 8px;" src="' . $par_image[$k] . '" />
        </a>
        <img style="cursor:pointer; margin:8px 0 0 0; vertical-align:top;" id="view_image_delete_' . $k . '" src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/delete.png" onclick=\'spider_catalog_remove(' . $k . ', "sel2");spider_catalog_delete_view_image(' . $k . ');\'>';
      $k++;
    }
    else {
      $image_items .= '
        <input type="text" class="form-text" style="width:200px;" id="inp_sel2_' . $k . '" value="" onChange=\'spider_catalog_add("sel2")\' />
        <img src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/delete.png" style="cursor:pointer;" onclick=\'spider_catalog_remove(' . $k . ', "sel2");\'><br />';
      $k = 1000;
    }
  }
  $show_images .= '</div>';
  $image_items .= '</div>
                   <input type="hidden" name="images" id="hid_sel2" value="' . $product_images . '" />
                   <div class="description">' . t('Enter image url.') . '</div>';
  $form['#attributes'] = array('enctype' => 'multipart/form-data');
  $form['product_add_image_button'] = array(
    '#prefix' => $image_items . '<div id="input_file_container"></div>',
    '#type' => 'button',
    '#value' => t('Add Image'),
    '#attributes' => array('onclick' => 'spider_catalog_add_upload_box(); return false;'),
    '#suffix' => $show_images,
  );
  $message_for_without_editor = '
    <div class="messages error" style="width:590px;">
      ' . t('To show HTML editor download "tinymce" library from !url and extract it into "sites/all/libraries/tinymce" directory.', array('!url' => l(t('here'), 'http://github.com/downloads/tinymce/tinymce/tinymce_3.5.7.zip'))) . '
    </div>';
  $form['product_short_description'] = array(
    '#prefix' => (file_exists("sites/all/libraries/tinymce/jscripts/tiny_mce/tiny_mce.js") ? '' : $message_for_without_editor) . '<div style="width:650px;">',
    '#type' => 'textarea',
    '#title' => t('Short Description'),
    '#default_value' => $product_short_description,
    '#attributes' => array('class' => array('spider_catalog_editor')),
    '#description' => t('Short description will be shown in the product list.'),
    '#resizable' => FALSE,
  );
  $form['product_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => $product_description,
    '#attributes' => array('class' => array('spider_catalog_editor')),
    '#resizable' => FALSE,
    '#suffix' => '</div>',
  );
  if (!isset($_POST['category_id'])) {
    $script_tag = 'script';
  }
  else {
    $script_tag = 'webdorado';
  }
  $param_items = '';
  $params_array = explode('#@@@#', $product_param);
  foreach ($params_array as $key => $params) {
    $param = explode("@@:@@", $params);
    if ($param[0] != '') {
      if (!isset($param[1])) {
        $param[1] = '';
      }
      $param_items .= '<strong>' . $param[0] . '</strong><div id="sel1_' . $param[0] . '">';
      $par = explode("#***#", $param[1]);
      $param_items .= '<' . $script_tag . '>
        var product_parameters_array = "' . $param[1] . '";
        parameters0["sel1_' . $param[0] . '"] = product_parameters_array.split("#***#");</' . $script_tag . '>';
      $k = 0;
      while ($k < 1000) {
        if (isset($par[$k]) && $par[$k] != '') {
          $param_items .= '
            <input type="text" class="form-text" style="width:200px;" id="inp_sel1_' . $param[0] . '_' . $k . '" value="' . $par[$k] . '" onChange=\'spider_catalog_add("sel1_' . $param[0] . '");\' />
            <img src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/delete.png" style="cursor:pointer;" onclick=\'spider_catalog_remove(' . $k . ', "sel1_' . $param[0] . '");\'><br />';
          $k++;
        }
        else {
          $param_items .= '
            <input type="text" class="form-text" style="width:200px;" id="inp_sel1_' . $param[0] . '_' . $k . '" value="" onChange=\'spider_catalog_add("sel1_' . $param[0] . '")\' />
            <img src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/delete.png" style="cursor:pointer;" onclick=\'spider_catalog_remove(' . $k . ', "sel1_' . $param[0] . '");\'><br />';
          $k = 1000;
        }
      }
      $param_items .= '</div>
                       <input type="hidden" name="param_' . $key . '" id="hid_sel1_' . $param[0] . '" value="' . $param[1] . '" />
                       <div class="description">' . t('Enter') . ' ' . $param[0] . '</div>';
    }
  }
  $form['product_published'] = array(
    '#prefix' => '<div id="category_parameters">' . $param_items . '</div>',
    '#type' => 'radios',
    '#title' => t('Published'),
    '#default_value' => $product_published,
    '#options' => array('1' => t('Yes'), '0' => t('No')),
  );
  $form['product_save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('spider_catalog_product_save'),
  );
  $form['product_apply'] = array(
    '#type' => 'submit',
    '#value' => t('Apply'),
    '#submit' => array('spider_catalog_product_apply'),
  );
  $form['product_cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#attributes' => array('onclick' => 'document.getElementById("edit-product-name").setAttribute("style", "color:rgba(255, 0, 0, 0)");document.getElementById("edit-product-name").setAttribute("value", "cancel");'),
    '#submit' => array('spider_catalog_product_cancel'),
  );
  // Check if post max size is smaller than this form post.
  $post_max_size_error = error_get_last();
  if ($post_max_size_error) {
    drupal_set_message($post_max_size_error['message'] . t('. Change "post_max_size" in .htaccess file.'), 'error', FALSE);
  }
  return $form;
}

/**
 * Save product.
 */
function spider_catalog_product_save($form, &$form_state) {
  $product_name = $form_state['values']['product_name'];
  $product_category_id = $form_state['values']['product_categories'];
  $product_price = $form_state['values']['product_price'];
  $product_market_price = $form_state['values']['product_market_price'];
  $dir = 'public://';
  $upload_event = array();
  if ($_FILES) {
    foreach ($_FILES['files']['name'] as $key => $value) {
      if (drupal_substr($key, 0, 12) == 'image_upload') {
        $image_name = $_FILES['files']['name'][$key];
        $image_size = $_FILES['files']['size'][$key];
        $image_type = $_FILES['files']['type'][$key];
        $validators = array(
          'file_validate_extensions' => array('jpg jpeg png gif'),
        );
        $image_file = file_save_upload($key, $validators, $dir);
        $image_file->status = FILE_STATUS_PERMANENT;
        file_save($image_file);
        if ($image_file) {
          $pos = drupal_strlen($key) - strrpos($key, '_') - 1;
          $index = drupal_substr($key, -$pos);
          $upload_event[$index] = base_path() . 'sites/default/files/' . $image_name . '#***#';
        }
      }
    }
  }
  // Array to string.
  if (!isset($items)) {
    $items = '';
  }
  // Add to files list old uploaded files.
  foreach ($_POST as $key => $value) {
    if (drupal_substr($key, 0, 10) == 'delete_img') {
      $items .= $value;
    }
  }
  // Add to files list new uploaded files.
  foreach ($upload_event as $upload_event_value) {
    $items .= $upload_event_value;
  }
  $product_description = $form_state['values']['product_description'];
  $product_short_description = $form_state['values']['product_short_description'];
  if (isset($_POST['images'])) {
    $images_array = check_plain($_POST['images']);
  }
  else {
    $images_array = '';
  }
  $images_array .= $items;
  $params = db_query("SELECT param FROM {spider_catalog_product_categories} WHERE id=:id", array(':id' => $product_category_id))->fetchField();
  $params_array = explode('#***#', $params);
  $parameters_array = '';
  foreach ($params_array as $key => $param_name) {
    if (isset($_POST['param_' . $key]) && ($param_name != '')) {
      $parameters_array .= $param_name . '@@:@@' . check_plain($_POST['param_' . $key]) . '#@@@#';
    }
  }
  $product_published = $form_state['values']['product_published'];

  if (isset($_GET['product_id']) && ($_GET['product_id'] != '')) {
    $product_id = check_plain($_GET['product_id']);
    db_query("UPDATE {spider_catalog_products} SET 
      name=:name,
      category_id=:category_id,
      cost=:cost,
      market_cost=:market_cost,
      image_url=:image_url,
      description=:description,
      short_description=:short_description,
      param=:param,
      published=:published WHERE id=:id", array(
        ':name' => $product_name,
        ':category_id' => $product_category_id,
        ':cost' => $product_price,
        ':market_cost' => $product_market_price,
        ':image_url' => $images_array,
        ':description' => $product_description,
        ':short_description' => $product_short_description,
        ':param' => $parameters_array,
        ':published' => $product_published,
        ':id' => $product_id));
  }
  else {
    db_insert('spider_catalog_products')
      ->fields(array(
        'name' => $product_name,
        'category_id' => $product_category_id,
        'cost' => $product_price,
        'market_cost' => $product_market_price,
        'image_url' => $images_array,
        'description' => $product_description,
        'short_description' => $product_short_description,
        'param' => $parameters_array,
        'ordering' => 0,
        'published' => $product_published,
        ))
      ->execute();
  }
  drupal_set_message(t('Your product successfully saved.'), 'status', FALSE);
  $form_state['redirect'] = url('admin/settings/spider_catalog/products', array('absolute' => TRUE));
}

/**
 * Apply product.
 */
function spider_catalog_product_apply($form, &$form_state) {
  $product_name = $form_state['values']['product_name'];
  $product_category_id = $form_state['values']['product_categories'];
  $product_price = $form_state['values']['product_price'];
  $product_market_price = $form_state['values']['product_market_price'];
  $dir = 'public://';
  $upload_event = array();
  if ($_FILES) {
    foreach ($_FILES['files']['name'] as $key => $value) {
      if (drupal_substr($key, 0, 12) == 'image_upload') {
        $image_name = $_FILES['files']['name'][$key];
        $image_size = $_FILES['files']['size'][$key];
        $image_type = $_FILES['files']['type'][$key];
        $validators = array(
          'file_validate_extensions' => array('jpg jpeg png gif'),
        );
        $image_file = file_save_upload($key, $validators, $dir);
        $image_file->status = FILE_STATUS_PERMANENT;
        file_save($image_file);
        if ($image_file) {
          $pos = drupal_strlen($key) - strrpos($key, '_') - 1;
          $index = drupal_substr($key, -$pos);
          $upload_event[$index] = base_path() . 'sites/default/files/' . $image_name . '#***#';
        }
      }
    }
  }
  // Array to string.
  if (!isset($items)) {
    $items = '';
  }
  // Add to files list old uploaded files.
  foreach ($_POST as $key => $value) {
    if (drupal_substr($key, 0, 10) == 'delete_img') {
      $items .= $value;
    }
  }
  // Add to files list new uploaded files.
  foreach ($upload_event as $upload_event_value) {
    $items .= $upload_event_value;
  }
  $product_description = $form_state['values']['product_description'];
  $product_short_description = $form_state['values']['product_short_description'];
  if (isset($_POST['images'])) {
    $images_array = check_plain($_POST['images']);
  }
  else {
    $images_array = '';
  }
  $images_array .= $items;
  $params = db_query("SELECT param FROM {spider_catalog_product_categories} WHERE id=:id", array(':id' => $product_category_id))->fetchField();
  $params_array = explode('#***#', $params);
  $parameters_array = '';
  foreach ($params_array as $key => $param_name) {
    if (isset($_POST['param_' . $key]) && ($param_name != '')) {
      $parameters_array .= $param_name . '@@:@@' . check_plain($_POST['param_' . $key]) . '#@@@#';
    }
  }
  $product_published = $form_state['values']['product_published'];

  if (isset($_GET['product_id']) && ($_GET['product_id'] != '')) {
    $product_id = check_plain($_GET['product_id']);
    db_query("UPDATE {spider_catalog_products} SET 
      name=:name,
      category_id=:category_id,
      cost=:cost,
      market_cost=:market_cost,
      image_url=:image_url,
      description=:description,
      short_description=:short_description,
      param=:param,
      published=:published WHERE id=:id", array(
        ':name' => $product_name,
        ':category_id' => $product_category_id,
        ':cost' => $product_price,
        ':market_cost' => $product_market_price,
        ':image_url' => $images_array,
        ':description' => $product_description,
        ':short_description' => $product_short_description,
        ':param' => $parameters_array,
        ':published' => $product_published,
        ':id' => $product_id));
    drupal_set_message(t('Your product successfully updated.'), 'status', FALSE);
  }
  else {
    db_insert('spider_catalog_products')
      ->fields(array(
        'name' => $product_name,
        'category_id' => $product_category_id,
        'cost' => $product_price,
        'market_cost' => $product_market_price,
        'image_url' => $images_array,
        'description' => $product_description,
        'short_description' => $product_short_description,
        'param' => $parameters_array,
        'ordering' => 0,
        'published' => $product_published,
        ))
      ->execute();
    $product_id = db_query("SELECT MAX(id) FROM {spider_catalog_products}")->fetchField();
    drupal_set_message(t('Your product successfully saved.'), 'status', FALSE);
  }
  $form_state['redirect'] = url('admin/settings/spider_catalog/products/edit', array('query' => array('product_id' => $product_id), 'absolute' => TRUE));
}

/**
 * Cancel product save.
 */
function spider_catalog_product_cancel($form, &$form_state) {
  $form_state['redirect'] = url('admin/settings/spider_catalog/products', array('absolute' => TRUE));
}

/**
 * Edit product ratings.
 */
function spider_catalog_product_edit_ratings() {
  if (isset($_GET['product_id'])) {
    $product_id = check_plain($_GET['product_id']);
    $product_name = db_query("SELECT name FROM {spider_catalog_products} WHERE id=:id", array(':id' => $product_id))->fetchField();
  }
  else {
    $product_id = 0;
    $product_name = '';
  }
  if (db_query("SELECT * FROM {spider_catalog_product_votes} WHERE id=:id", array(':id' => $product_id))) {
    $header = array(
      'id' => array('data' => t('ID'), 'field' => 'n.id'),
      'remote_ip' => array('data' => t('Remote IP'), 'field' => 'n.remote_ip'),
      'vote_value' => array('data' => t('Vote Value'), 'field' => 'n.vote_value'),
      'delete' => array('data' => t('Delete')),
    );
    $rates_array = array(1 => 1, 2 => 2, 3 => 3, 4 => 4, 5 => 5);
    $options = array();
    $ratings_ids = db_select('spider_catalog_product_votes', 'n')
      ->fields('n', array('id'))
      ->condition('n.product_id', $product_id, '=')
      ->extend('TableSort')
      ->orderByHeader($header)
      ->extend('PagerDefault')
      ->limit(20)
      ->execute()
      ->fetchCol();
    foreach ($ratings_ids as $ratings_id) {
      $row = db_query("SELECT * FROM {spider_catalog_product_votes} WHERE id=:id", array(':id' => $ratings_id))->fetchObject();
      $options[$ratings_id] = array(
        'id' => $ratings_id,
        'remote_ip' => $row->remote_ip,
        'vote_value' => array(
          'data' => array(
            '#type' => 'select',
            '#value' => $row->vote_value,
            '#options' => $rates_array,
            '#attributes' => array('name' => 'spider_catalog_products_votes_' . $ratings_id),
          ),
        ),
        'delete' => array(
          'data' => array(
            '#type' => 'link',
            '#title' => t('Delete'),
            '#href' => url('admin/settings/spider_catalog/products/edit/ratings/delete', array(
              'query' => array(
                'product_id' => $product_id, 'ratings_id' => $ratings_id),
              'absolute' => TRUE)),
          ),
        ),
      );
    }
    $form['ratings_delete'] = array(
      '#prefix' => '<h3>' . t('Edit @product_name ratings', array('@product_name' => check_plain($product_name))) . '</h3><br />',
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('spider_catalog_ratings_delete'),
      '#attributes' => array('onclick' => 'if (!confirm(Drupal.t("Do you want to delete selected rates?"))) {return false;}'),
    );
    $form['ratings_save'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#submit' => array('spider_catalog_ratings_save'),
    );
    $form['ratings_apply'] = array(
      '#type' => 'submit',
      '#value' => t('Apply'),
      '#submit' => array('spider_catalog_ratings_apply'),
    );
    $form['ratings_cancel'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
      '#submit' => array('spider_catalog_ratings_cancel'),
    );
    $form['ratings_table'] = array(
      '#type' => 'tableselect',
      '#js_select' => TRUE,
      '#header' => $header,
      '#options' => $options,
      '#empty' => t('No rating available for this product.'),
      '#suffix' => theme('pager', array('tags' => array())),
    );
    return $form;
  }
}

/**
 * Delete selected ratings.
 */
function spider_catalog_ratings_delete($form, &$form_state) {
  if (db_query("SELECT id FROM {spider_catalog_product_votes}")) {
    $ratings_ids_col = db_query("SELECT id FROM {spider_catalog_product_votes}")->fetchCol();
    $flag = FALSE;
    foreach ($ratings_ids_col as $ratings_id) {
      if (isset($_POST['ratings_table'][$ratings_id])) {
        $flag = TRUE;
        db_query("DELETE FROM {spider_catalog_product_votes} WHERE id=:id", array(':id' => $ratings_id));
        drupal_set_message(t('Selected ratings successfully deleted.'), 'status', FALSE);
      }
    }
    if ($flag == FALSE) {
      drupal_set_message(t('You must check a rating.'), 'warning', FALSE);
    }
    if (isset($_GET['product_id'])) {
      $product_id = check_plain($_GET['product_id']);
      drupal_goto(url('admin/settings/spider_catalog/products/edit/ratings', array('query' => array('product_id' => $product_id), 'absolute' => TRUE)));
    }
  }
}

/**
 * Delete rating.
 */
function spider_catalog_rating_delete() {
  if (isset($_GET['ratings_id'])) {
    $ratings_id = check_plain($_GET['ratings_id']);
    db_query("DELETE FROM {spider_catalog_product_votes} WHERE id=:id", array(':id' => $ratings_id));
  }
  else {
    $ratings_id = 0;
  }
  if (isset($_GET['product_id'])) {
    $product_id = check_plain($_GET['product_id']);
    drupal_set_message(t('Rating successfully deleted.'), 'status', FALSE);
    drupal_goto(url('admin/settings/spider_catalog/products/edit/ratings', array('query' => array('product_id' => $product_id), 'absolute' => TRUE)));
  }
}

/**
 * Save editing product reviews.
 */
function spider_catalog_ratings_save($form, &$form_state) {
  if (isset($_GET['product_id'])) {
    $product_id = check_plain($_GET['product_id']);
  }
  else {
    $product_id = 0;
  }
  if (db_query("SELECT * FROM {spider_catalog_product_votes} WHERE product_id=:product_id", array(':product_id' => $product_id))) {
    $ratings_ids = db_query("SELECT id FROM {spider_catalog_product_votes} WHERE product_id=:product_id", array(':product_id' => $product_id))->fetchCol();
    foreach ($ratings_ids as $ratings_id) {
      if (isset($_POST['spider_catalog_products_votes_' . $ratings_id])) {
        $vote = check_plain($_POST['spider_catalog_products_votes_' . $ratings_id]);
        db_query("UPDATE {spider_catalog_product_votes} SET vote_value=:vote_value WHERE id=:id", array(':vote_value' => $vote, ':id' => $ratings_id));
      }
      drupal_set_message(t('Changes to Ratings successfully saved.'), 'status', FALSE);
    }
    $form_state['redirect'] = url('admin/settings/spider_catalog/products/edit', array('query' => array('product_id' => $product_id), 'absolute' => TRUE));
  }
}

/**
 * Apply editing product reviews.
 */
function spider_catalog_ratings_apply($form, &$form_state) {
  if (isset($_GET['product_id'])) {
    $product_id = check_plain($_GET['product_id']);
  }
  else {
    $product_id = 0;
  }
  if (db_query("SELECT * FROM {spider_catalog_product_votes} WHERE product_id=:product_id", array(':product_id' => $product_id))) {
    $ratings_ids = db_query("SELECT id FROM {spider_catalog_product_votes} WHERE product_id=:product_id", array(':product_id' => $product_id))->fetchCol();
    foreach ($ratings_ids as $ratings_id) {
      if (isset($_POST['spider_catalog_products_votes_' . $ratings_id])) {
        $vote = check_plain($_POST['spider_catalog_products_votes_' . $ratings_id]);
        db_query("UPDATE {spider_catalog_product_votes} SET vote_value=:vote_value WHERE id=:id", array(':vote_value' => $vote, ':id' => $ratings_id));
      }
      drupal_set_message(t('Changes to Ratings successfully saved.'), 'status', FALSE);
    }
  }
  drupal_goto(url('admin/settings/spider_catalog/products/edit/ratings', array('query' => array('product_id' => $product_id), 'absolute' => TRUE)));
}

/**
 * Cancel editing product reviews.
 */
function spider_catalog_ratings_cancel($form, &$form_state) {
  if (isset($_GET['product_id'])) {
    $product_id = check_plain($_GET['product_id']);
    $form_state['redirect'] = url('admin/settings/spider_catalog/products/edit', array('query' => array('product_id' => $product_id), 'absolute' => TRUE));
  }
}

/**
 * Edit product reviews.
 */
function spider_catalog_product_edit_reviews() {
  if (isset($_GET['product_id'])) {
    $product_id = check_plain($_GET['product_id']);
    $product_name = db_query("SELECT name FROM {spider_catalog_products} WHERE id=:id", array(':id' => $product_id))->fetchField();
  }
  else {
    $product_id = 0;
    $product_name = '';
  }
  if (db_query("SELECT * FROM {spider_catalog_product_reviews} WHERE id=:id", array(':id' => $product_id))) {
    $header = array(
      'id' => array('data' => t('ID'), 'field' => 'n.id'),
      'remote_ip' => array('data' => t('Remote IP'), 'field' => 'n.remote_ip'),
      'name' => array('data' => t('Name'), 'field' => 'n.name'),
      'content' => array('data' => t('Message'), 'field' => 'n.content'),
      'delete' => array('data' => t('Delete')),
    );
    $options = array();
    $reviews_ids = db_select('spider_catalog_product_reviews', 'n')
      ->fields('n', array('id'))
      ->condition('n.product_id', $product_id, '=')
      ->extend('TableSort')
      ->orderByHeader($header)
      ->extend('PagerDefault')
      ->limit(20)
      ->execute()
      ->fetchCol();
    foreach ($reviews_ids as $reviews_id) {
      $row = db_query("SELECT * FROM {spider_catalog_product_reviews} WHERE id=:id", array(':id' => $reviews_id))->fetchObject();
      $options[$reviews_id] = array(
        'id' => $reviews_id,
        'remote_ip' => $row->remote_ip,
        'name' => $row->name,
        'content' => $row->content,
        'delete' => array(
          'data' => array(
            '#type' => 'link',
            '#title' => t('Delete'),
            '#href' => url('admin/settings/spider_catalog/products/edit/reviews/delete', array(
              'query' => array(
                'product_id' => $product_id, 'reviews_id' => $reviews_id),
              'absolute' => TRUE)),
          ),
        ),
      );
    }
    $form['reviews_delete'] = array(
      '#prefix' => '<h3>' . t('Edit @product_name reviews', array('@product_name' => check_plain($product_name))) . '</h3><br />',
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('spider_catalog_reviews_delete'),
      '#attributes' => array('onclick' => 'if (!confirm(Drupal.t("Do you want to delete selected reviews?"))) {return false;}'),
    );
    $form['reviews_cancel'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
      '#submit' => array('spider_catalog_ratings_cancel'),
    );
    $form['reviews_table'] = array(
      '#type' => 'tableselect',
      '#js_select' => TRUE,
      '#header' => $header,
      '#options' => $options,
      '#empty' => t('No review available for this product.'),
      '#suffix' => theme('pager', array('tags' => array())),
    );
    return $form;
  }
}

/**
 * Delete selected reviews.
 */
function spider_catalog_reviews_delete($form, &$form_state) {
  if (db_query("SELECT id FROM {spider_catalog_product_reviews}")) {
    $reviews_ids_col = db_query("SELECT id FROM {spider_catalog_product_reviews}")->fetchCol();
    $flag = FALSE;
    foreach ($reviews_ids_col as $reviews_id) {
      if (isset($_POST['reviews_table'][$reviews_id])) {
        $flag = TRUE;
        db_query("DELETE FROM {spider_catalog_product_reviews} WHERE id=:id", array(':id' => $reviews_id));
        drupal_set_message(t('Selected reviews successfully deleted.'), 'status', FALSE);
      }
    }
    if ($flag == FALSE) {
      drupal_set_message(t('You must check a review.'), 'warning', FALSE);
    }
    if (isset($_GET['product_id'])) {
      $product_id = check_plain($_GET['product_id']);
      drupal_goto(url('admin/settings/spider_catalog/products/edit/reviews', array('query' => array('product_id' => $product_id), 'absolute' => TRUE)));
    }
  }
}

/**
 * Delete review.
 */
function spider_catalog_review_delete() {
  if (isset($_GET['reviews_id'])) {
    $reviews_id = check_plain($_GET['reviews_id']);
    db_query("DELETE FROM {spider_catalog_product_reviews} WHERE id=:id", array(':id' => $reviews_id));
  }
  else {
    $reviews_id = 0;
  }
  if (isset($_GET['product_id'])) {
    $product_id = check_plain($_GET['product_id']);
    drupal_set_message(t('Review successfully deleted.'), 'status', FALSE);
    drupal_goto(url('admin/settings/spider_catalog/products/edit/reviews', array('query' => array('product_id' => $product_id), 'absolute' => TRUE)));
  }
}
