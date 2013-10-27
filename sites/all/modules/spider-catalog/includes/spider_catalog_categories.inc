<?php
/**
 * @file
 * Spider Catalog module catalog categories.
 */

/**
 * Menu loader callback. Load catalog categories.
 */
function spider_catalog_categories() {
  $form = array();
  $free_version = '<a href="http://web-dorado.com/products/drupal-catalog.html" target="_blank" style="color:red; text-decoration:none; float:right;">
                    <img src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/header.png" border="0" alt="www.web-dorado.com" width="215"><br />
                  <div style="float:right;">' . t('Get the full version') . '&nbsp;&nbsp;&nbsp;&nbsp;</div>
                  </a>';
  $form['fieldset_category_buttons'] = array(
    '#prefix' => $free_version,
    '#type' => 'fieldset',
  );
  $form['fieldset_category_buttons']['publish_category'] = array(
    '#type' => 'submit',
    '#submit' => array('spider_catalog_categories_publish'),
    '#value' => t('Publish'),
  );
  $form['fieldset_category_buttons']['unpublish_category'] = array(
    '#type' => 'submit',
    '#submit' => array('spider_catalog_categories_unpublish'),
    '#value' => t('Unpublish'),
  );
  $form['fieldset_category_buttons']['delete_category'] = array(
    '#type' => 'submit',
    '#submit' => array('spider_catalog_categories_delete'),
    '#value' => t('Delete'),
    '#attributes' => array('onclick' => 'if (!confirm(Drupal.t("Do you want to delete selected categories?"))) {return false;}'),
  );
  $form['fieldset_category_buttons']['save_order'] = array(
    '#type' => 'submit',
    '#submit' => array('spider_catalog_save_ordering'),
    '#value' => t('Save order'),
  );
  $form['fieldset_category_buttons']['new_category'] = array(
    '#prefix' => l(t('New'), url('admin/settings/spider_catalog/categories/edit', array('absolute' => TRUE))),
  );
  $form['fieldset_search_categories_name'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search categories by name'),
    '#collapsible' => TRUE,
    '#collapsed' => ((variable_get('spider_catalog_search_categories_name', '') == '') ? TRUE : FALSE),
  );
  $form['fieldset_search_categories_name']['search_categories_name'] = array(
    '#type' => 'textfield',
    '#size' => 25,
    '#default_value' => variable_get('spider_catalog_search_categories_name', ''),
  );
  $form['fieldset_search_categories_name']['search_categories'] = array(
    '#type' => 'submit',
    '#submit' => array('spider_catalog_search_categories_name'),
    '#value' => t('Go'),
  );
  $form['fieldset_search_categories_name']['reset_categories'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('spider_catalog_reset_categories_name'),
  );

  drupal_add_tabledrag('categories_table-components', 'order', 'sibling', 'weight');
  $header = array(
    'checkbox' => array('class' => 'select-all'),
    'id' => array('data' => t('ID'), 'field' => 'n.id'),
    'name' => array('data' => t('Name'), 'field' => 'n.name'),
    'description' => array('data' => t('Description'), 'field' => 'n.description'),
    'order' => array(
      'data' => t('Order'),
      'field' => 'n.ordering',
      'sort' => 'asc'),
    'published' => array('data' => t('Published')),
    'delete' => array('data' => t('Delete')),
  );
  $options = array();
  $categories_ids = db_select('spider_catalog_product_categories', 'n')
    ->fields('n', array('id'))
    ->condition('n.name', '%' . db_like(variable_get('spider_catalog_search_categories_name', '')) . '%', 'LIKE')
    ->extend('TableSort')
    ->orderByHeader($header)
    ->extend('PagerDefault')
    ->limit(20)
    ->execute()
    ->fetchCol();
  foreach ($categories_ids as $category_id) {
    $row = db_query("SELECT * FROM {spider_catalog_product_categories} WHERE id=:id", array(':id' => $category_id))->fetchObject();
    if ($row->published) {
      $publish_unpublish_png = 'publish.png';
      $publish_unpublish_function = 'unpublish';
    }
    else {
      $publish_unpublish_png = 'unpublish.png';
      $publish_unpublish_function = 'publish';
    }
    variable_set('spider_catalog_category_id', $category_id);
    $options[$category_id] = array(
      'checkbox' => array(
        'data' => array(
          '#type' => 'checkbox',
          '#attributes' => array('name' => 'spider_catalog_category_check_' . $category_id),
        ),
      ),
      'id' => $category_id,
      'name' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => $row->name,
          '#href' => url('admin/settings/spider_catalog/categories/edit', array('query' => array('category_id' => $category_id), 'absolute' => TRUE)),
        ),
      ),
      'description' => $row->description,
      'order' => array(
        'data' => array(
          '#type' => 'textfield',
          '#size' => 3,
          '#value' => $row->ordering,
          '#attributes' => array('name' => 'spider_catalog_categories_order_' . $category_id, 'class' => array('weight')),
        ),
      ),
      'published' => l('<img src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/' . $publish_unpublish_png . '" />', url('admin/settings/spider_catalog/categories/' . $publish_unpublish_function, array('query' => array('category_id' => $category_id), 'absolute' => TRUE)), array('html' => TRUE)),
      'delete' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => t('Delete'),
          '#href' => url('admin/settings/spider_catalog/categories/delete', array('query' => array('category_id' => $category_id), 'absolute' => TRUE)),
        ),
      ),
    );
    $options[$category_id]['#attributes'] = array('class' => array('draggable'));
  }
  $form['categories_table'] = array(
    '#type' => 'tableselect',
    '#js_select' => TRUE,
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No categories available.'),
    '#suffix' => theme('pager', array('tags' => array())),
    '#attributes' => array('id' => 'categories_table-components'),
  );

  foreach ($categories_ids as $category_id) {
    $form['categories_table'][$category_id]['#disabled'] = TRUE;
  }
  return $form;
}

/**
 * Search in categories.
 */
function spider_catalog_search_categories_name($form, &$form_state) {
  if ($form_state['values']['search_categories_name'] != '') {
    variable_set('spider_catalog_search_categories_name', $form_state['values']['search_categories_name']);
  }
  else {
    variable_set('spider_catalog_search_categories_name', '');
  }
  $form_state['redirect'] = url('admin/settings/spider_catalog/categories', array('absolute' => TRUE));
}

/**
 * Reset categories.
 */
function spider_catalog_reset_categories_name($form, &$form_state) {
  variable_set('spider_catalog_search_categories_name', '');
  $form_state['redirect'] = url('admin/settings/spider_catalog/categories', array('absolute' => TRUE));
}

/**
 * Publish selected categories.
 */
function spider_catalog_categories_publish($form, &$form_state) {
  if (db_query("SELECT id FROM {spider_catalog_product_categories}")) {
    $category_ids_col = db_query("SELECT id FROM {spider_catalog_product_categories}")->fetchCol();
    $flag = FALSE;
    foreach ($category_ids_col as $category_id) {
      if (isset($_POST['spider_catalog_category_check_' . $category_id])) {
        $flag = TRUE;
        db_query("UPDATE {spider_catalog_product_categories} SET published=:published WHERE id=:id", array(':published' => 1, ':id' => $category_id));
      }
    }
    if ($flag == FALSE) {
      drupal_set_message(t('You must select at least one category.'), 'warning', FALSE);
    }
    else {
      drupal_set_message(t('Categories successfully published.'), 'status', FALSE);
    }
  }
}

/**
 * Publish category.
 */
function spider_catalog_category_publish() {
  if (isset($_GET['category_id'])) {
    $category_id = check_plain($_GET['category_id']);
    db_query("UPDATE {spider_catalog_product_categories} SET published=:published WHERE id=:id", array(':published' => 1, ':id' => $category_id));
  }
  drupal_set_message(t('Category successfully published.'), 'status', FALSE);
  drupal_goto(url('admin/settings/spider_catalog/categories', array('absolute' => TRUE)));
}

/**
 * Unpublish selected categories.
 */
function spider_catalog_categories_unpublish($form, &$form_state) {
  if (db_query("SELECT id FROM {spider_catalog_product_categories}")) {
    $category_ids_col = db_query("SELECT id FROM {spider_catalog_product_categories}")->fetchCol();
    $flag = FALSE;
    foreach ($category_ids_col as $category_id) {
      if (isset($_POST['spider_catalog_category_check_' . $category_id])) {
        $flag = TRUE;
        db_query("UPDATE {spider_catalog_product_categories} SET published=:published WHERE id=:id", array(':published' => 0, ':id' => $category_id));
      }
    }
    if ($flag == FALSE) {
      drupal_set_message(t('You must select at least one category.'), 'warning', FALSE);
    }
    else {
      drupal_set_message(t('Categories successfully unpublished.'), 'status', FALSE);
    }
  }
}

/**
 * Unpublish category.
 */
function spider_catalog_category_unpublish() {
  if (isset($_GET['category_id'])) {
    $category_id = check_plain($_GET['category_id']);
    db_query("UPDATE {spider_catalog_product_categories} SET published=:published WHERE id=:id", array(':published' => 0, ':id' => $category_id));
  }
  drupal_set_message(t('Category successfully unpublished.'), 'status', FALSE);
  drupal_goto(url('admin/settings/spider_catalog/categories', array('absolute' => TRUE)));
}

/**
 * Delete selected categories.
 */
function spider_catalog_categories_delete($form, &$form_state) {
  if (db_query("SELECT id FROM {spider_catalog_product_categories}")) {
    $category_ids_col = db_query("SELECT id FROM {spider_catalog_product_categories}")->fetchCol();
    $flag = FALSE;
    foreach ($category_ids_col as $category_id) {
      if (isset($_POST['spider_catalog_category_check_' . $category_id])) {
        $flag = TRUE;
        db_query("DELETE FROM {spider_catalog_product_categories} WHERE id=:id", array(':id' => $category_id));
        // Set deleted category product category uncategorized.
        db_query("UPDATE {spider_catalog_products} SET category_id=0 WHERE category_id=:category_id", array(':category_id' => $category_id));
        drupal_set_message(t('Selected categories successfully deleted.'), 'status', FALSE);
      }
    }
    if ($flag == FALSE) {
      drupal_set_message(t('You must select at least one category.'), 'warning', FALSE);
    }
    else {
      drupal_set_message(t('Categories successfully deleted.'), 'status', FALSE);
    }
  }
}

/**
 * Delete category.
 */
function spider_catalog_category_delete() {
  if (isset($_GET['category_id'])) {
    $category_id = check_plain($_GET['category_id']);
    db_query("DELETE FROM {spider_catalog_product_categories} WHERE id=:id", array(':id' => $category_id));
    // Set deleted category product category uncategorized.
    db_query("UPDATE {spider_catalog_products} SET category_id=0 WHERE category_id=:category_id", array(':category_id' => $category_id));
  }
  drupal_set_message(t('Category successfully deleted.'), 'status', FALSE);
  drupal_goto(url('admin/settings/spider_catalog/categories', array('absolute' => TRUE)));
}

/**
 * Save ordering.
 */
function spider_catalog_save_ordering($form, &$form_state) {
  $categories_ids = db_query("SELECT id FROM {spider_catalog_product_categories}")->fetchCol();
  foreach ($categories_ids as $category_id) {
    if (isset($_POST['spider_catalog_categories_order_' . $category_id])) {
      $ordering = check_plain($_POST['spider_catalog_categories_order_' . $category_id]);
      db_query("UPDATE {spider_catalog_product_categories} SET ordering=:ordering WHERE id=:id", array(':ordering' => $ordering, ':id' => $category_id));
    }
    drupal_set_message(t('Category ordering successfully saved.'), 'status', FALSE);
  }
}

/**
 * Add or edit category.
 */
function spider_catalog_category_edit() {
  drupal_add_js(drupal_get_path('module', 'spider_catalog') . '/js/spider_catalog_attach_file.js');
  drupal_add_js(drupal_get_path('module', 'spider_catalog') . '/js/spider_catalog_params.js');
  drupal_add_js(array(
    'spider_catalog' => array(
      'delete_png_url' => base_path() . drupal_get_path('module', 'spider_catalog') . '/images/',
    ),
    ),
    'setting');
  if (isset($_GET['category_id'])) {
    $category_id = check_plain($_GET['category_id']);
    $row = db_query("SELECT * FROM {spider_catalog_product_categories} WHERE id=:id", array(':id' => $category_id))->fetchObject();
    $category_name = $row->name;
    $category_images = $row->category_image_url;
    $images_array = explode('#***#', $row->category_image_url);
    $category_description = $row->description;
    $category_param = $row->param;
    $category_published = $row->published;
  }
  else {
    $category_id = '';
    $category_name = '';
    $category_images = '';
    $images_array = '';
    $category_description = '';
    $category_param = '';
    $category_published = 1;
  }
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
  drupal_add_js('
    var category_images_array = "' . $category_images . '";
    var parameters0 = new Array();
    parameters0["sel2"] = category_images_array.split("#***#");', array('type' => 'inline'));
  $form['category_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
    '#default_value' => $category_name,
    '#size' => 15,
  );
  $par_image = explode("#***#", $category_images);
  $k = 0;
  $image_items = '<strong>' . t('Images') . '</strong><div id="sel2">';
  $show_images = '<div id="images_div" style="width:470px;">';
  while ($k < 1000) {
    if (isset($par_image[$k]) && $par_image[$k] != '') {
      $image_items .= '
        <input type="text" class="form-text" style="width:200px;" id="inp_sel2_' . $k . '" value="' . $par_image[$k] . '" onChange=\'spider_catalog_add("sel2")\' />
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
                   <input type="hidden" name="images" id="hid_sel2" value="' . $category_images . '" />
                   <div class="description">' . t('Enter image url.') . '</div>';
  $form['#attributes'] = array('enctype' => 'multipart/form-data');
  $form['category_add_image_button'] = array(
    '#prefix' => $image_items . '<div id="input_file_container"></div>',
    '#type' => 'button',
    '#value' => t('Add Image'),
    '#attributes' => array('onclick' => 'spider_catalog_add_upload_box(); return false;'),
    '#suffix' => $show_images,
  );
  $message_for_without_editor = '
    <div class="messages error" style="width:650px;">
      ' . t('To show HTML editor download "tinymce" library from !url and extract it into "sites/all/libraries/tinymce" directory.', array('!url' => l(t('here'), 'http://github.com/downloads/tinymce/tinymce/tinymce_3.5.7.zip'))) . '
    </div>';
  $form['category_description'] = array(
    '#prefix' => (file_exists("sites/all/libraries/tinymce/jscripts/tiny_mce/tiny_mce.js") ? '' : $message_for_without_editor) . '<div style="width:650px;">',
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => $category_description,
    '#attributes' => array('class' => array('spider_catalog_editor')),
    '#resizable' => FALSE,
    '#suffix' => '</div>',
  );
  drupal_add_js('
    var category_parameters_array = "' . $category_param . '";
    /*var parameters0 = new Array();*/
    parameters0["sel1"] = category_parameters_array.split("#***#");
    /*parameters0["sel1"].push("");*/', array('type' => 'inline'));
  $form['category_hidden'] = array(
    '#type' => 'hidden',
    '#value' => $category_param,
  );
  $par = explode("#***#", $category_param);
  $k = 0;
  $param_items = '<strong>' . t('Parameters') . '</strong><div id="sel1">';
  while ($k < 1000) {
    if (isset($par[$k]) && $par[$k] != '') {
      $param_items .= '
        <input type="text" class="form-text" style="width:200px;" id="inp_sel1_' . $k . '" value="' . $par[$k] . '" onChange=\'spider_catalog_add("sel1")\' />
        <img src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/delete.png" style="cursor:pointer;" onclick=\'spider_catalog_remove(' . $k . ', "sel1");\'><br />';
      $k++;
    }
    else {
      $param_items .= '
        <input type="text" class="form-text" style="width:200px;" id="inp_sel1_' . $k . '" value="" onChange=\'spider_catalog_add("sel1")\' />
        <img src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/delete.png" style="cursor:pointer;" onclick=\'spider_catalog_remove(' . $k . ', "sel1");\'><br />';
      $k = 1000;
    }
  }
  $param_items .= '</div>
                   <input type="hidden" name="param" id="hid_sel1" value="' . $category_param . '" />
                   <div class="description">' . t('Enter parameter name.') . '</div>';
  $form['category_published'] = array(
    '#prefix' => $param_items,
    '#type' => 'radios',
    '#title' => t('Published'),
    '#default_value' => $category_published,
    '#options' => array('1' => t('Yes'), '0' => t('No')),
  );
  $form['category_save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('spider_catalog_category_save'),
  );
  $form['category_apply'] = array(
    '#type' => 'submit',
    '#value' => t('Apply'),
    '#submit' => array('spider_catalog_category_apply'),
  );
  $form['category_cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#attributes' => array('onclick' => 'document.getElementById("edit-category-name").setAttribute("style", "color:rgba(255, 0, 0, 0)");document.getElementById("edit-category-name").setAttribute("value", "cancel");'),
    '#submit' => array('spider_catalog_category_cancel'),
  );
  // Check if post max size is smaller than this form post.
  $post_max_size_error = error_get_last();
  if ($post_max_size_error) {
    drupal_set_message($post_max_size_error['message'] . t('. Change "post_max_size" in .htaccess file.'), 'error', FALSE);
  }
  return $form;
}

/**
 * Save category.
 */
function spider_catalog_category_save($form, &$form_state) {
  $category_name = $form_state['values']['category_name'];
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
  $category_description = $form_state['values']['category_description'];
  if (isset($_POST['images'])) {
    $images_array = check_plain($_POST['images']);
  }
  else {
    $images_array = '';
  }
  $images_array .= $items;
  if (isset($_POST['param'])) {
    $parameters_array = check_plain($_POST['param']);
  }
  else {
    $parameters_array = '';
  }
  $category_published = $form_state['values']['category_published'];

  if (isset($_GET['category_id'])) {
    $category_id = check_plain($_GET['category_id']);
    $parameters = explode('#***#', $parameters_array);
    $cat_old_params = explode('#***#', $form_state['values']['category_hidden']);
    $params = db_query("SELECT param FROM {spider_catalog_products} WHERE category_id=:category_id", array(':category_id' => $category_id))->fetchCol();
    foreach ($params as $param) {
      $new_param = $param;
      foreach ($cat_old_params as $key => $cat_old_param) {
        if ($cat_old_param != '') {
          $new_param = str_replace($cat_old_param . '@@:@@', $parameters[$key] . '@@:@@', $new_param);
        }
      }
      db_query("UPDATE {spider_catalog_products} SET param='" . $new_param . "' WHERE param='" . $param . "'");
    }
    db_query("UPDATE {spider_catalog_product_categories} SET 
      name=:name,
      category_image_url=:category_image_url,
      description=:description,
      param=:param,
      published=:published WHERE id=:id", array(
        ':name' => $category_name,
        ':category_image_url' => $images_array,
        ':description' => $category_description,
        ':param' => $parameters_array,
        ':published' => $category_published,
        ':id' => $category_id));
  }
  else {
    db_insert('spider_catalog_product_categories')
      ->fields(array(
        'name' => $category_name,
        'category_image_url' => $images_array,
        'description' => $category_description,
        'param' => $parameters_array,
        'ordering' => 0,
        'published' => $category_published,
        ))
      ->execute();
  }
  drupal_set_message(t('Your category successfully saved.'), 'status', FALSE);
  $form_state['redirect'] = url('admin/settings/spider_catalog/categories', array('absolute' => TRUE));
}

/**
 * Apply category.
 */
function spider_catalog_category_apply($form, &$form_state) {
  $category_name = $form_state['values']['category_name'];
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
    elseif (drupal_substr($key, 0, 14) == 'delete_vidHttp') {
      $items .= $value;
    }
    elseif (drupal_substr($key, 0, 17) == 'delete_vidYoutube') {
      $items .= $value;
    }
  }
  // Add to files list new uploaded files.
  foreach ($upload_event as $upload_event_value) {
    $items .= $upload_event_value;
  }
  if (isset($_POST['images'])) {
    $images_array = check_plain($_POST['images']);
  }
  else {
    $images_array = '';
  }
  $images_array .= $items;
  $category_description = $form_state['values']['category_description'];
  if (isset($_POST['param'])) {
    $parameters_array = check_plain($_POST['param']);
  }
  else {
    $parameters_array = '';
  }
  $category_published = $form_state['values']['category_published'];

  if (isset($_GET['category_id'])) {
    $category_id = check_plain($_GET['category_id']);
    $parameters = explode('#***#', $parameters_array);
    $cat_old_params = explode('#***#', $form_state['values']['category_hidden']);
    $params = db_query("SELECT param FROM {spider_catalog_products} WHERE category_id=:category_id", array(':category_id' => $category_id))->fetchCol();
    foreach ($params as $param) {
      $new_param = $param;
      foreach ($cat_old_params as $key => $cat_old_param) {
        if ($cat_old_param != '') {
          $new_param = str_replace($cat_old_param . '@@:@@', $parameters[$key] . '@@:@@', $new_param);
        }
      }
      db_query("UPDATE {spider_catalog_products} SET param='" . $new_param . "' WHERE param='" . $param . "'");
    }
    db_query("UPDATE {spider_catalog_product_categories} SET 
      name=:name,
      category_image_url=:category_image_url,
      description=:description,
      param=:param,
      published=:published WHERE id=:id", array(
        ':name' => $category_name,
        ':category_image_url' => $images_array,
        ':description' => $category_description,
        ':param' => $parameters_array,
        ':published' => $category_published,
        ':id' => $category_id));
    drupal_set_message(t('Your category successfully updated.'), 'status', FALSE);
  }
  else {
    db_insert('spider_catalog_product_categories')
      ->fields(array(
        'name' => $category_name,
        'category_image_url' => $images_array,
        'description' => $category_description,
        'param' => $parameters_array,
        'ordering' => 0,
        'published' => $category_published,
        ))
      ->execute();
    $category_id = db_query("SELECT MAX(id) FROM {spider_catalog_product_categories}")->fetchField();
    drupal_set_message(t('Your category successfully saved.'), 'status', FALSE);
  }
  $form_state['redirect'] = url('admin/settings/spider_catalog/categories/edit', array('query' => array('category_id' => $category_id), 'absolute' => TRUE));
}

/**
 * Cancel category save.
 */
function spider_catalog_category_cancel($form, &$form_state) {
  $form_state['redirect'] = url('admin/settings/spider_catalog/categories', array('absolute' => TRUE));
}
