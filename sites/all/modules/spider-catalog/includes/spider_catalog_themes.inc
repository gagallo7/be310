<?php
/**
 * @file
 * Catalog themes.
 */

/**
 * Menu loader callback. Load a Spider Catalog themes.
 */
function spider_catalog_themes() {
  $form = array();
  $free_version = '
    <a href="http://web-dorado.com/products/drupal-catalog.html" target="_blank" style="color:red; text-decoration:none; float:right;">
      <img src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/header.png" border="0" alt="www.web-dorado.com" width="215"><br />
    <div style="float:right;">' . t('Get the full version') . '&nbsp;&nbsp;&nbsp;&nbsp;</div>
    </a>';
  $form['delete_themes'] = array(
    '#prefix' => $free_version,
    '#type' => 'submit',
    '#submit' => array('spider_catalog_delete_themes'),
    '#value' => t('Delete'),
    '#attributes' => array('onclick' => 'if (!confirm(Drupal.t("Do you want to delete selected themes?"))) {return false;}'),
  );
  $form['new_theme'] = array(
    '#prefix' => l(t('New'), url('admin/settings/spider_catalog/themes/edit', array('absolute' => TRUE))),
  );
  $header = array(
    'id' => array('data' => t('ID'), 'field' => 'n.id'),
    'title' => array('data' => t('Title'), 'field' => 'n.title'),
    'delete' => array('data' => t('Delete')),
  );
  $options = array();
  $theme_ids = db_select('spider_catalog_themes', 'n')
    ->fields('n', array('id'))
    ->extend('TableSort')
    ->orderByHeader($header)
    ->extend('PagerDefault')
    ->limit(20)
    ->execute()
    ->fetchCol();
  foreach ($theme_ids as $theme_id) {
    $row = db_query('SELECT * FROM {spider_catalog_themes} WHERE id=:id', array(':id' => $theme_id))->fetchObject();
    $options[$theme_id] = array(
      'id' => $theme_id,
      'title' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => $row->title,
          '#href' => url('admin/settings/spider_catalog/themes/edit', array('query' => array('theme_id' => $theme_id), 'absolute' => TRUE)),
        ),
      ),
    );
    if ($row->default_settings != 1) {
      $options[$theme_id]['delete'] = array(
        'data' => array(
          '#type' => 'link',
          '#title' => t('Delete'),
          '#href' => url('admin/settings/spider_catalog/themes/delete', array('query' => array('theme_id' => $theme_id), 'absolute' => TRUE)),
        ),
      );
    }
    else {
      $options[$theme_id]['delete'] = '';
    }
  }
  $form['themes_table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No theme available.'),
    '#suffix' => theme('pager', array('tags' => array())),
  );
  foreach ($theme_ids as $theme_id) {
    if ($theme_id <= 1) {
      $form['themes_table'][$theme_id]['#disabled'] = TRUE;
    }
  }
  return $form;
}

/**
 * Delete selected themes.
 */
function spider_catalog_delete_themes($form, &$form_state) {
  if (db_query("SELECT id FROM {spider_catalog_themes}")) {
    $theme_ids_col = db_query("SELECT id FROM {spider_catalog_themes}")->fetchCol();
    $flag = FALSE;
    foreach ($theme_ids_col as $theme_id) {
      if (isset($_POST['themes_table'][$theme_id])) {
        $flag = TRUE;
        db_query("DELETE FROM {spider_catalog_themes} WHERE id=:id", array(':id' => $theme_id));
        drupal_set_message(t('Selected themes successfully deleted.'), 'status', FALSE);
      }
    }
    if ($flag == FALSE) {
      drupal_set_message(t('You must select at least one theme.'), 'warning', FALSE);
    }
  }
}

/**
 * Delete theme.
 */
function spider_catalog_theme_delete() {
  if (isset($_GET['theme_id'])) {
    $theme_id = check_plain($_GET['theme_id']);
  }
  db_query("DELETE FROM {spider_catalog_themes} WHERE id=:id", array(':id' => $theme_id));
  drupal_set_message(t('Theme successfully deleted.'), 'status', FALSE);
  drupal_goto(url('admin/settings/spider_catalog/themes', array('absolute' => TRUE)));
}

/**
 * Add or edit theme.
 */
function spider_catalog_theme_edit() {
  drupal_add_css(drupal_get_path('module', 'spider_catalog') . '/css/spider_catalog_theme.css');
  drupal_add_js(drupal_get_path('module', 'spider_catalog') . '/js/spider_catalog_theme_reset.js');
  if (!file_exists("sites/all/libraries/jscolor/jscolor.js")) {
    $message = t('Download "jscolor" library from !jscolor_url and extract it to "sites/all/libraries/jscolor" directory.', array(
      '!jscolor_url' => l(t('here'), 'http://jscolor.com/release/jscolor-1.4.0.zip')));
    drupal_set_message(filter_xss($message), 'warning', FALSE);
  }
  else {
    drupal_add_js('sites/all/libraries/jscolor/jscolor.js');
    $jscolor_class = 'color';
  }
  if (isset($_GET['theme_id'])) {
    $theme_id = check_plain($_GET['theme_id']);
    $title_theme = '';
  }
  else {
    $theme_id = 1;
    $title_theme = 'new ';
    drupal_set_message(t("You can't create a new theme in free version."), 'error', FALSE);
    drupal_goto(url('admin/settings/spider_catalog/themes', array('absolute' => TRUE)));
  }
  $themeinf = db_query("SELECT * FROM {spider_catalog_themes} WHERE id=:id", array(':id' => $theme_id))->fetchObject();
  $title_theme = $title_theme . $themeinf->title;
  $rounded_corners = $themeinf->rounded_corners;
  $background_color = $themeinf->background_color;
  $parameters_background_ccolor_1 = $themeinf->parameters_background_ccolor_1;
  $parameters_background_ccolor_2 = $themeinf->parameters_background_ccolor_2;
  $border_style = $themeinf->border_style;
  $border_width = $themeinf->border_width;
  $border_color = $themeinf->border_color;
  $text_color = $themeinf->text_color;
  $color_of_parameter_values = $themeinf->color_of_parameter_values;
  $hyperlink_color = $themeinf->hyperlink_color;
  $price_color = $themeinf->price_color;
  $title_color = $themeinf->title_color;
  $title_background_color = $themeinf->title_background_color;
  $color_of_text_of_buttons = $themeinf->color_of_text_of_buttons;
  $buttons_background_color = $themeinf->buttons_background_color;
  $rating_star_design = $themeinf->rating_star_design;
  $category_picture_width = $themeinf->category_picture_width;
  $category_picture_height = $themeinf->category_picture_height;
  $category_title_size = $themeinf->category_title_size;
  $count_of_products_in_the_row = $themeinf->count_of_products_in_the_row;
  $count_of_rows_in_the_page = $themeinf->count_of_rows_in_the_page;
  $product_cell_width = $themeinf->product_cell_width;
  $product_cell_height = $themeinf->product_cell_height;
  $picture_width = $themeinf->picture_width;
  $picture_height = $themeinf->picture_height;
  $text_size = $themeinf->text_size;
  $price_size = $themeinf->price_size;
  $title_size = $themeinf->title_size;
  $show_category = $themeinf->show_category;
  $show_parameters = $themeinf->show_parameters;
  $count_of_products_in_the_page_list = $themeinf->count_of_products_in_the_page_list;
  $picture_width_list = $themeinf->picture_width_list;
  $picture_height_list = $themeinf->picture_height_list;
  $text_size_list = $themeinf->text_size_list;
  $price_size_list = $themeinf->price_size_list;
  $show_category_list = $themeinf->show_category_list;
  $show_parameters_list = $themeinf->show_parameters_list;
  $show_description_list = $themeinf->show_description_list;
  $picture_width_product = $themeinf->picture_width_product;
  $picture_height_product = $themeinf->picture_height_product;
  $text_size_product = $themeinf->text_size_product;
  $price_size_product = $themeinf->price_size_product;
  $title_size_product = $themeinf->title_size_product;
  $background_color_review = $themeinf->background_color_review;
  $color_of_review_texts = $themeinf->color_of_review_texts;
  $background_color_review_author = $themeinf->background_color_review_author;
  $background_color_review_text = $themeinf->background_color_review_text;
  $number_of_reviews_per_page = $themeinf->number_of_reviews_per_page;
  $default_settings = $themeinf->default_settings;
  $form = array();
  if ($default_settings != '1' || !isset($_GET['theme_id'])) {
    $diable_title_field = FALSE;
    $themes = db_query("SELECT id,title FROM {spider_catalog_themes} WHERE default_settings=:default_settings ORDER BY title", array(':default_settings' => 1))->fetchAllKeyed();
    $themes[0] = '';
    $form['theme_select'] = array(
      '#type' => 'fieldset',
      '#attributes' => array(
        'class' => array('spider_catalog_theme_select_fieldset'),
        'id' => 'spider_catalog_theme_select_fieldset',
      ),
    );
    $form['theme_select']['default_themes'] = array(
      '#type' => 'select',
      '#title' => t('Inherit from theme'),
      '#options' => $themes,
      '#attributes' => array('onchange' => 'spider_catalog_set_theme();'),
      '#default_value' => 0,
    );
  }
  else {
    $diable_title_field = TRUE;
  }
  $free_version = '
    <div class="messages warning">Themes are disabled in free version. If you need this functionality, you need to buy the commercial version.</div>
    <a href="http://web-dorado.com/products/drupal-catalog.html" target="_blank" style="color:red; text-decoration:none; float:right;width:100%;">
      <img style="float:right;" src="' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/header.png" border="0" alt="www.web-dorado.com" width="215"><br /><br /><br />
      <div style="float:right;">' . t('Get the full version') . '&nbsp;&nbsp;&nbsp;&nbsp;</div>
    </a>';
  $form['theme_global'] = array(
    '#prefix' => $free_version,
    '#type' => 'fieldset',
    '#title' => t('Global Parameters'),
    '#attributes' => array(
      'class' => array('spider_catalog_theme_global_fielset'),
      'id' => 'theme_global',
    ),
  );
  $form['theme_global']['theme_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#required' => TRUE,
    '#default_value' => $title_theme,
    '#disabled' => $diable_title_field,
    '#size' => 25,
  );
  $form['theme_global']['rounded_corners'] = array(
    '#type' => 'radios',
    '#title' => t('Rounded Corners'),
    '#default_value' => $rounded_corners,
    '#options' => array('1' => t('Enable'), '0' => t('Disable')),
  );
  $form['theme_global']['background_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Background color'),
    '#default_value' => $background_color,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_global']['parameters_background_ccolor_1'] = array(
    '#type' => 'textfield',
    '#title' => t('Parameters Background color 1'),
    '#default_value' => $parameters_background_ccolor_1,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_global']['parameters_background_ccolor_2'] = array(
    '#type' => 'textfield',
    '#title' => t('Parameters Background color 2'),
    '#default_value' => $parameters_background_ccolor_2,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $border_style_select = array(
    'solid' => t('Solid'),
    'double' => t('Double'),
    'dashed' => t('Dashed'),
    'dotted' => t('Dotted'),
    'groove' => t('Groove'),
    'inset' => t('Inset'),
    'outset' => t('Outset'),
    'ridge' => t('Ridge'),
  );
  $form['theme_global']['border_style'] = array(
    '#type' => 'select',
    '#title' => t('Border Style'),
    '#default_value' => $border_style,
    '#options' => $border_style_select,
  );
  $form['theme_global']['border_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Border Width'),
    '#default_value' => $border_width,
    '#size' => 25,
  );
  $form['theme_global']['border_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Border Color'),
    '#default_value' => $border_color,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_global']['text_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Text Color'),
    '#default_value' => $text_color,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_global']['color_of_parameter_values'] = array(
    '#type' => 'textfield',
    '#title' => t('Color of Parameter Values'),
    '#default_value' => $color_of_parameter_values,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_global']['hyperlink_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Hyperlink Color'),
    '#default_value' => $hyperlink_color,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_global']['price_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Price Color'),
    '#default_value' => $price_color,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_global']['title_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Title Color'),
    '#default_value' => $title_color,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_global']['title_background_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Title Background color'),
    '#default_value' => $title_background_color,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_global']['color_of_text_of_buttons'] = array(
    '#type' => 'textfield',
    '#title' => t('Color of text of buttons'),
    '#default_value' => $color_of_text_of_buttons,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_global']['buttons_background_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Buttons Background color'),
    '#default_value' => $buttons_background_color,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $image_url = base_path() . drupal_get_path('module', 'spider_catalog') . '/images/star_images/star';
  $stars_array = array(
    1 => '<img class="spider_catalog_rating" src="' . $image_url . '1.png" />',
    2 => '<img class="spider_catalog_rating" src="' . $image_url . '2.png" "/>',
    3 => '<img class="spider_catalog_rating;" src="' . $image_url . '3.png" />',
    4 => '<img class="spider_catalog_rating" src="' . $image_url . '4.png" />',
    5 => '<img class="spider_catalog_rating" src="' . $image_url . '5.png" />',
  );
  $form['theme_global']['rating_star_design'] = array(
    '#type' => 'radios',
    '#title' => t('Rating Star Design'),
    '#default_value' => $rating_star_design,
    '#options' => $stars_array,
  );
  $form['theme_global']['category_picture_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Category Picture Width'),
    '#default_value' => $category_picture_width,
    '#size' => 25,
  );
  $form['theme_global']['category_picture_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Category Picture Height'),
    '#default_value' => $category_picture_height,
    '#size' => 25,
  );
  $form['theme_global']['category_title_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Category title size'),
    '#default_value' => $category_title_size,
    '#size' => 25,
  );
  if (isset($_GET['theme_id']) && $default_settings == 1) {
    $reset_button = '
      <div id="reset_theme_id" onclick="spider_catalog_reset_theme_' . $theme_id . '();" style="cursor:pointer; width:170px; height:35px; color:#ffffff; font-size:15px; background-repeat:no-repeat; background-image:url(\'' . base_path() . drupal_get_path('module', 'spider_catalog') . '/images/reset_theme.png\')">
        <p style="font-weight: bold; padding-left: 51px; padding-top: 7px; margin:0px;">' . t('Reset Theme') . '</p>
      </div>';
    $form['theme_global']['reset_button'] = array(
      '#prefix' => $reset_button,
    );
  }
  $form['theme_cells_page_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cells Page Options'),
    '#attributes' => array(
      'class' => array('spider_catalog_theme_cells_page_options_fielset'),
      'id' => 'theme_cells_page_options',
    ),
  );
  $form['theme_cells_page_options']['count_of_products_in_the_row'] = array(
    '#type' => 'textfield',
    '#title' => t('Count of Products in the Row'),
    '#default_value' => $count_of_products_in_the_row,
    '#size' => 25,
  );
  $form['theme_cells_page_options']['count_of_rows_in_the_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Count of Rows in the Page'),
    '#default_value' => $count_of_rows_in_the_page,
    '#size' => 25,
  );
  $form['theme_cells_page_options']['product_cell_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Product Cell Width'),
    '#default_value' => $product_cell_width,
    '#size' => 25,
  );
  $form['theme_cells_page_options']['product_cell_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Product Cell Height'),
    '#default_value' => $product_cell_height,
    '#size' => 25,
  );
  $form['theme_cells_page_options']['picture_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Picture Width'),
    '#default_value' => $picture_width,
    '#size' => 25,
  );
  $form['theme_cells_page_options']['picture_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Picture Height'),
    '#default_value' => $picture_height,
    '#size' => 25,
  );
  $form['theme_cells_page_options']['text_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Text Size'),
    '#default_value' => $text_size,
    '#size' => 25,
  );
  $form['theme_cells_page_options']['price_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Price Size'),
    '#default_value' => $price_size,
    '#size' => 25,
  );
  $form['theme_cells_page_options']['title_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Title Size'),
    '#default_value' => $title_size,
    '#size' => 25,
  );
  $form['theme_cells_page_options']['show_category'] = array(
    '#type' => 'radios',
    '#title' => t('Show Category'),
    '#default_value' => $show_category,
    '#options' => array('1' => t('Enable'), '0' => t('Disable')),
  );
  $form['theme_cells_page_options']['show_parameters'] = array(
    '#type' => 'radios',
    '#title' => t('Show Parameters'),
    '#default_value' => $show_parameters,
    '#options' => array('1' => t('Enable'), '0' => t('Disable')),
  );
  $form['theme_list_page_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('List Page Options'),
    '#attributes' => array(
      'class' => array('spider_catalog_theme_list_page_options_fielset'),
      'id' => 'theme_list_page_options',
    ),
  );
  $form['theme_list_page_options']['count_of_products_in_the_page_list'] = array(
    '#type' => 'textfield',
    '#title' => t('Count of Products in the page'),
    '#default_value' => $count_of_products_in_the_page_list,
    '#size' => 25,
  );
  $form['theme_list_page_options']['picture_width_list'] = array(
    '#type' => 'textfield',
    '#title' => t('Picture Width'),
    '#default_value' => $picture_width_list,
    '#size' => 25,
  );
  $form['theme_list_page_options']['picture_height_list'] = array(
    '#type' => 'textfield',
    '#title' => t('Picture Height'),
    '#default_value' => $picture_height_list,
    '#size' => 25,
  );
  $form['theme_list_page_options']['text_size_list'] = array(
    '#type' => 'textfield',
    '#title' => t('Text Size'),
    '#default_value' => $text_size_list,
    '#size' => 25,
  );
  $form['theme_list_page_options']['price_size_list'] = array(
    '#type' => 'textfield',
    '#title' => t('Price Size'),
    '#default_value' => $price_size_list,
    '#size' => 25,
  );
  $form['theme_list_page_options']['show_category_list'] = array(
    '#type' => 'radios',
    '#title' => t('Show Category'),
    '#default_value' => $show_category_list,
    '#options' => array('1' => t('Enable'), '0' => t('Disable')),
  );
  $form['theme_list_page_options']['show_parameters_list'] = array(
    '#type' => 'radios',
    '#title' => t('Show Parameters'),
    '#default_value' => $show_parameters_list,
    '#options' => array('1' => t('Enable'), '0' => t('Disable')),
  );
  $form['theme_list_page_options']['show_description_list'] = array(
    '#type' => 'radios',
    '#title' => t('Show Description'),
    '#default_value' => $show_description_list,
    '#options' => array('1' => t('Enable'), '0' => t('Disable')),
  );
  $form['theme_product_page_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Product Page Options'),
    '#attributes' => array(
      'class' => array('spider_catalog_theme_product_page_options_fielset'),
      'id' => 'theme_product_page_options',
    ),
  );
  $form['theme_product_page_options']['picture_width_product'] = array(
    '#type' => 'textfield',
    '#title' => t('Picture Width'),
    '#default_value' => $picture_width_product,
    '#size' => 25,
  );
  $form['theme_product_page_options']['picture_height_product'] = array(
    '#type' => 'textfield',
    '#title' => t('Picture Height'),
    '#default_value' => $picture_height_product,
    '#size' => 25,
  );
  $form['theme_product_page_options']['text_size_product'] = array(
    '#type' => 'textfield',
    '#title' => t('Text Size'),
    '#default_value' => $text_size_product,
    '#size' => 25,
  );
  $form['theme_product_page_options']['price_size_product'] = array(
    '#type' => 'textfield',
    '#title' => t('Price Size'),
    '#default_value' => $price_size_product,
    '#size' => 25,
  );
  $form['theme_product_page_options']['title_size_product'] = array(
    '#type' => 'textfield',
    '#title' => t('Title Size'),
    '#default_value' => $title_size_product,
    '#size' => 25,
  );
  $form['theme_product_page_options']['background_color_review'] = array(
    '#type' => 'textfield',
    '#title' => t('Background Color of "Add your review here" section'),
    '#default_value' => $background_color_review,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_product_page_options']['color_of_review_texts'] = array(
    '#type' => 'textfield',
    '#title' => t('Color of Review Texts'),
    '#default_value' => $color_of_review_texts,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_product_page_options']['background_color_review_author'] = array(
    '#type' => 'textfield',
    '#title' => t('Background Color of Review Author section'),
    '#default_value' => $background_color_review_author,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_product_page_options']['background_color_review_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Background Color of Review text'),
    '#default_value' => $background_color_review_text,
    '#size' => 25,
    '#attributes' => array('class' => array('color')),
  );
  $form['theme_product_page_options']['number_of_reviews_per_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of reviews per page'),
    '#default_value' => $number_of_reviews_per_page,
    '#size' => 25,
  );
  $form['theme_save'] = array(
    '#type' => 'submit',
    '#prefix' => '<div style="float:left; bottom:0px; clear:left;">',
    '#value' => t('Save'),
    '#attributes' => array('onclick' => 'alert(Drupal.t("You cant save the changes in free version."));return false;'),
  );
  $form['theme_apply'] = array(
    '#type' => 'submit',
    '#value' => t('Apply'),
    '#attributes' => array('onclick' => 'alert(Drupal.t("You cant save the changes in free version."));return false;'),
  );
  $form['theme_cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array('spider_catalog_theme_cancel'),
    '#suffix' => '</div>',
  );
  return $form;
}

/**
 * Cancel theme save.
 */
function spider_catalog_theme_cancel($form, &$form_state) {
  $form_state['redirect'] = url('admin/settings/spider_catalog/themes', array('absolute' => TRUE));
}
